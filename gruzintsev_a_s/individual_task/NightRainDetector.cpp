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

    Mat corrected = correctBrightness(channels[0]);
    Mat overbrighted = corrected > 250;

    if (showPictures)imshow("Source", source);
//    if (showPictures)imshow("Brighness", channels[0]);
//    if (showPictures)imshow("Corrected", corrected);
//    if (showPictures)imshow("Ovebrighted", overbrighted);

    showHist(corrected);

    if (night) {
        cout << " Night ";
    } else {
        cout << " Day ";
    }



//    Mat cutted = raw(Rect(0, int(raw.rows * 2.0 / 10), raw.cols, int(raw.rows * 6.0 / 10)));
//    Mat source;
//
//    resize(cutted, source, cv::Size(int(cutted.cols * SCALE_KOEF), int(cutted.rows * SCALE_KOEF)));
//
//    Mat result(source.rows, source.cols, CV_8UC3);
//
//    source.copyTo(result(cv::Rect(0, 0, source.cols, source.rows)));
//
//    Mat sourceInHSV;
//    cvtColor(source, sourceInHSV, CV_BGR2HSV);
//
////    showChannels(channels);
//
//    Mat gammaCorrected = applyGammaCorrection(channels[2]);
//    imshow("Gamma correction", gammaCorrected);

    bool answer = false;

//    if (isNight(gammaCorrected)) {
//        cout << " night";
//
//        Mat bluredImage = apply(gammaCorrected, [](Mat &source, Mat &processed) {
//            medianBlur(source, processed, 7);
//        });
////    imshow("Blured", bluredImage);
//
//        Mat morphologed = apply(bluredImage, [](Mat &source, Mat &processed) {
//            auto kernel = Mat::ones(5, 5, CV_8UC1);
//            morphologyEx(source, processed, MORPH_OPEN, kernel);
//        });
////    imshow("Morphologed", morphologed);
//
//        Mat thresholded = morphologed > 200;
//
//
////    Mat colored(raw.size(), CV_8UC3);
//        auto contours = findLights(thresholded);
//        drawLightsRects(result, contours);
//
//        answer = checkBlinksBelowRects(gammaCorrected, contours);
//        if (showPictures) imshow("Threashholded", gammaCorrected);
//    } else {
//        cout << " day " << false << endl;
//        answer = false;
//    }

//    if (showPictures)imshow("RESULT", result);

    cout << endl;

    return answer;
}

void NightRainDetector::filterWithCustomKernel(cv::Mat &mat) {
    cv::Mat filtered1;
    cv::Mat filtered2;
    int SAME_DEPTH = -1;

    cv::Mat kernel = customKernel();
    cv::Point center = cv::Point(-1, -1);

    filter2D(mat, filtered1, SAME_DEPTH, customKernel(), center);
//        filter2D(mat, filtered2, SAME_DEPTH, customKernel(), center);

    imshow("Custom filter", filtered1);
}

cv::Mat NightRainDetector::horizontalDerivative(cv::Mat &mat) {
    cv::Mat kernel = horizontalDerivativeKernel();
    cv::Mat derivative;

    int SAME_DEPTH = -1;
    cv::Point center = cv::Point(-1, -1);

    cv::Mat blured;
    medianBlur(mat, blured, 7);

    filter2D(blured, derivative, SAME_DEPTH, kernel, center);

    cv::Mat normalizedDerivative;
    cv::Mat derivativeSquare;
    pow(derivative, 2, derivativeSquare);
    derivativeSquare.convertTo(normalizedDerivative, CV_8UC1);

    cv::Mat morphologed;

    cv::Mat kernel2 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4), cv::Point(-1, -1));

    morphologyEx(normalizedDerivative, morphologed, cv::MORPH_OPEN, kernel2);

    return morphologed;
}

Mat NightRainDetector::verticalDerivative(Mat &mat) {
    Mat result(mat.rows, mat.cols * 3, CV_8UC1);
    vector<Mat> channels;
    split(mat, channels);

    Mat kernel = horizontalDerivativeKernel();
    Mat derivative;

    int SAME_DEPTH = -1;
    Point center = Point(-1, -1);

    Mat blured;
    medianBlur(channels[2], blured, 7);

    filter2D(blured, derivative, SAME_DEPTH, kernel.t(), center);

    Mat normalizedDerivative;
    Mat derivativeSquare;
    pow(derivative, 2, derivativeSquare);
    derivativeSquare.convertTo(normalizedDerivative, CV_8UC1);

    return normalizedDerivative;
}

Mat NightRainDetector::horizontalDerivativeKernel() {
    Mat kernel(3, 3, CV_32FC1);

    kernel.at<float>(0, 0) = 1;
    kernel.at<float>(1, 0) = 1;
    kernel.at<float>(2, 0) = 1;

    kernel.at<float>(0, 1) = 0;
    kernel.at<float>(1, 1) = 0;
    kernel.at<float>(2, 1) = 0;

    kernel.at<float>(0, 2) = -1;
    kernel.at<float>(1, 2) = -1;
    kernel.at<float>(2, 2) = -1;

    return kernel;
}

Mat NightRainDetector::customKernel() {
    Mat kernel = Mat(5, 3, CV_32FC1);

    kernel.at<float>(0, 0) = -1.0f;
    kernel.at<float>(0, 2) = -1.0f;
    kernel.at<float>(1, 0) = -1.0f;
    kernel.at<float>(1, 2) = -1.0f;
    kernel.at<float>(3, 0) = -1.0f;
    kernel.at<float>(3, 2) = -1.0f;
    kernel.at<float>(4, 0) = -1.0f;
    kernel.at<float>(4, 2) = -1.0f;

    kernel.at<float>(2, 0) = 0.0f;
    kernel.at<float>(2, 2) = 0.0f;

    kernel.at<float>(0, 1) = 0.5f;
    kernel.at<float>(4, 1) = 0.5f;

    kernel.at<float>(1, 1) = 0.0f;
    kernel.at<float>(3, 1) = 0.0f;

    kernel.at<float>(2, 1) = 0.0f;

    return kernel / sum(kernel)[0];
}

bool NightRainDetector::isNight(Mat &mat) {
//    Mat topHalfOfSource = mat;//(Rect(0, 0, mat.cols, int(mat.cols * NIGHT_TOP_PART)));

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
            cout << " Median value: " << medianVal << " ";

            break;
        }
    }
//    return medianVal / maxValue < NIGHT_UP_THRESHOLD;
    return medianVal < 60;
}

vector<Mat> NightRainDetector::splitToChannels(cv::Mat &mat) {
    vector<Mat> channels;
    split(mat, channels);

    return channels;
}

void NightRainDetector::showChannels(std::vector<cv::Mat> &channels) {
    Mat result(channels[0].rows, channels[0].cols * 3, CV_8UC1);

    for (int i = 0; i < 3; i++) {
        channels[i].copyTo(result(Rect(channels[i].cols * i, 0, channels[i].cols, channels[i].rows)));
    }

    imshow("Channels", result);
}

Mat NightRainDetector::applyGammaCorrection(cv::Mat &oneChannelImage) {
    Mat lookUpTable(1, 256, CV_8U);
    uchar *p = lookUpTable.ptr();
    for (int i = 0; i < 256; ++i)
        p[i] = saturate_cast<uchar>(pow(i / 255.0, 2) * 255.0);
    Mat result = oneChannelImage.clone();
    LUT(oneChannelImage, lookUpTable, result);

    return result;
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

//        cout << " -> Next rect <- " << endl;
//        cout << "Proportion: " << proportion << endl;
//        cout << "Min width: " << minWidth << endl;
//        cout << "Max width: " << maxWidth << endl;

        if (mat.rows && proportion > 0.7 &&
            minWidth > 0.009 * SCALE_KOEF * mat.cols &&
            maxWidth < 0.1 * SCALE_KOEF * mat.cols) {
            rects.push_back(rect);
        }
    }

    return rects;
}

void NightRainDetector::drawLightsRects(Mat &threeChannelsMat, vector<Rect> &contours) {
    for (int i = 0; i < contours.size(); i++) {
        Scalar color = Scalar(0, 0, 255);
        rectangle(threeChannelsMat, contours[i], color, 3);
    }
}

bool NightRainDetector::checkBlinksBelowRects(cv::Mat &oneChannelMat, std::vector<cv::Rect> &rects) {
    bool almostOne = false;

    Mat filtered = oneChannelMat > 200;

    for (Rect rect:rects) {

        Rect areaToFind = Rect(rect.x, rect.y + rect.height, rect.width, oneChannelMat.rows - rect.y - rect.height);

        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;

        Mat findAreaMat = filtered(areaToFind);

        findContours(findAreaMat, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

        vector<Rect> highlightedRects;
        for (auto points:contours) {
            Rect boundingRect = cv::boundingRect(points);
            if (boundingRect.area() > 100 * SCALE_KOEF) {
                highlightedRects.push_back(boundingRect);
            }
        }

        almostOne = almostOne || highlightedRects.size() > 0;

        rectangle(oneChannelMat, areaToFind, Scalar(128), 2);
        rectangle(oneChannelMat, areaToFind, Scalar(128), 2);
    }

    cout << " " << almostOne << endl;

    return almostOne;
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
