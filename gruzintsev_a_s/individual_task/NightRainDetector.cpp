#include "NightRainDetector.h"

using namespace std;
using namespace cv;

bool NightRainDetector::detect(string path, bool showPictures) {
    Mat raw = imread(path, cv::IMREAD_COLOR);

    Mat source;
    resize(raw, source, cv::Size(int(raw.cols * SCALE_KOEF), int(raw.rows * SCALE_KOEF)));

    // There should be detection for night
    Mat sourceInHSV;
    cvtColor(source, sourceInHSV, CV_BGR2HSV);

    auto channels = splitToChannels(source);

    bool night = isNight(channels[0]);
    bool answer = false;

    if (showPictures){
        imshow("Source", source);
        showHist(channels[0]);
    }

    if (night) {
        cout << " [Night]:";

        Mat corrected = correctBrightness(channels[0]);
        Mat overbrighted = corrected > 250;

        Mat onlyLights = corrected > 245;
        auto lights = findLights(onlyLights);

        auto blinks = blinksBelowRects(channels[0], lights);

        answer = blinks.size() > 0;

        if (answer) {
            cout << "[TRUE]";
        } else {
            cout << "[FALSE]";
        }
    } else {
        cout << " [Day]  :[FALSE]";
    }

    cout << endl;

    return answer;
}

bool NightRainDetector::isNight(Mat &mat) {
    int maxValue = 255;
    float range[] = {0.0, (float) maxValue};
    const float *histRange = {range};
    bool uniform = true;
    bool accumulate = false;
    cv::Mat distribution;
    calcHist(&mat, 1, nullptr, cv::Mat(), distribution, 1, &maxValue, &histRange, uniform, accumulate);

    distribution.copyTo(distribution);
    for (int i = 1; i <= maxValue - 1; i++) {
        distribution.at<float>(i) += distribution.at<float>(i - 1);
    }
    distribution /= mat.total();

    double medianVal = distribution.at<float>(0);
    for (int i = 0; i <= maxValue - 1; i++) {
        if (distribution.at<float>(i) >= 0.5) {
            medianVal = i;
            break;
        }
    }
    return medianVal < 60;
}

vector<Mat> NightRainDetector::splitToChannels(cv::Mat &mat) {
    vector<Mat> channels;
    split(mat, channels);

    return channels;
}


vector<Rect> NightRainDetector::findLights(Mat &mat) {
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    findContours(mat, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    vector<Rect> rects;

    for (auto points :contours) {
        Rect rect = boundingRect(points);
        double minWidth = min(rect.height, rect.width);
        double maxWidth = max(rect.height, rect.width);
        double proportion = min(rect.height * 1.0 / rect.width, rect.width * 1.0 / rect.height);

        if (mat.rows && proportion > 0.7 &&
            minWidth > 0.009 * SCALE_KOEF * mat.cols &&
            maxWidth < 0.1 * SCALE_KOEF * mat.cols) {
            rects.push_back(rect);
        }
    }

    return rects;
}

vector<Rect> NightRainDetector::blinksBelowRects(cv::Mat &oneChannelMat, std::vector<cv::Rect> &rects) {
    vector<Rect> blinks;

    for (Rect rect:rects) {
        int additionalWidth = int(oneChannelMat.cols * ADDITIONAL_WIDTH);

        Rect areaToFind = Rect(max(rect.x - additionalWidth, 0),
                               rect.y + rect.height,
                               min(rect.x + rect.width + 2 * additionalWidth, oneChannelMat.cols) - rect.x,
                               oneChannelMat.rows - rect.y - rect.height);

        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;

        if (areaToFind.width == 0 || areaToFind.height == 0) {
            continue;
        }

        Mat area = oneChannelMat(Rect(areaToFind));
        Mat blured = apply(area, [](Mat &input, Mat &output) { medianBlur(input, output, 23); });

        Mat equalized = correctBrightness(blured);
        Mat ranged = equalized > 200;

        findContours(ranged, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

        for (auto points:contours) {
            Rect boundingRect = cv::boundingRect(points);
            if (boundingRect.area() > 100 * SCALE_KOEF) {
                Rect rect1(areaToFind.x + boundingRect.x, areaToFind.y + boundingRect.y, boundingRect.width,
                           boundingRect.height);

                if (rect1.width > areaToFind.width * 0.10 && rect1.height > 2 * rect1.width) {
                    blinks.push_back(rect1);
                    rectangle(ranged, boundingRect, Scalar(128), 2);
                }
            }
        }
    }

    return blinks;
}

cv::Mat NightRainDetector::correctBrightness(cv::Mat brighnessChannel) {

    Mat lookUpTable(1, 256, CV_8U);
    uchar *p = lookUpTable.ptr();
    Mat result = brighnessChannel.clone();

    Mat hist;

    int channels[] = {0};

    float range_0[] = {0, 255};
    const float *ranges[] = {range_0};

    int hbins = 255;
    int histSize[] = {hbins};

    calcHist(&brighnessChannel, 1, channels, Mat(), hist, 1, histSize, ranges);


    bool left = false;
    bool right = false;

    int quantileLeft = 0, quantileRight = 255;
    float sumLeft = 0, sumRight = 0;

    for (int i = 0; i < hist.size[0]; i++) {

        if (!left && sumLeft / (brighnessChannel.size[0] * brighnessChannel.size[1]) < 0.01) {
            sumLeft += hist.at<float>(i);
            quantileLeft = i;
        } else {
            left = true;

        }

        if (!right && sumRight / (brighnessChannel.size[0] * brighnessChannel.size[1]) < 0.01) {
            sumRight += hist.at<float>(hist.size[0] - 1 - i);
            quantileRight = hist.size[0] - 1 - i;
        } else {
            right = true;
        }
    }

    for (int i = 0; i < 256; ++i) {
        if (i < quantileLeft) {
            p[i] = (uchar) 0;
        } else if (i > quantileRight) {
            p[i] = (uchar) 255;
        } else {
            p[i] = (uchar) (255.0 / (quantileRight - quantileLeft) * (i - quantileLeft));
        }
    }

    LUT(brighnessChannel, lookUpTable, result);

    return result;
}

void NightRainDetector::showHist(cv::Mat oneChannel) {

    // Initialize parameters
    int histSize = 256;    // bin size
    float range[] = {0, 255};
    const float *ranges[] = {range};

    // Calculate histogram
    Mat hist;
    calcHist(&oneChannel, 1, 0, Mat(), hist, 1, &histSize, ranges, true, false);

    // Plot the histogram
    int hist_w = 512;
    int hist_h = 400;
    int bin_w = cvRound((double) hist_w / histSize);

    Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(0, 0, 0));
    normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

    for (int i = 1; i < histSize; i++) {
        line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(hist.at<float>(i - 1))),
             Point(bin_w * (i), hist_h - cvRound(hist.at<float>(i))),
             Scalar(255, 0, 0), 2, 8, 0);
    }

    imshow("Hist", histImage);
}
