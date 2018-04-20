#include "NightRainDetector.h"

using namespace std;
using namespace cv;

void NightRainDetector::detect(string path) {
    Mat raw = cv::imread(path, cv::IMREAD_COLOR);
    Mat source = cv::imread(path, cv::IMREAD_COLOR);
    resize(raw, source, cv::Size(int(raw.cols * SCALE_KOEF), int(raw.rows * SCALE_KOEF)));

    cv::Mat result(source.rows, source.cols, CV_8UC3);

    source.copyTo(result(cv::Rect(0, 0, source.cols, source.rows)));

    cv::Mat sourceInHSV;
    cvtColor(source, sourceInHSV, CV_BGR2HSV);

    auto channels = splitToChannels(sourceInHSV);
//    showChannels(channels);

    Mat gammaCorrected = applyGammaCorrection(channels[2]);
    imshow("Gamma correction", gammaCorrected);

    cv::Mat bluredImage = apply(gammaCorrected, [](Mat &source, Mat &processed) {
        medianBlur(source, processed, 7);
    });
    imshow("Blured", bluredImage);

    Mat thresholded = gammaCorrected > 250;
    imshow("Threashholded", thresholded);


    imshow("RESULT", result);
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
    Mat topHalfOfSource = mat(Rect(0, 0, mat.cols, int(mat.cols * NIGHT_TOP_PART)));

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
    return medianVal / maxValue < NIGHT_UP_THRESHOLD;
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

cv::Mat NightRainDetector::applyGammaCorrection(cv::Mat &oneChannelImage) {
    Mat lookUpTable(1, 256, CV_8U);
    uchar *p = lookUpTable.ptr();
    for (int i = 0; i < 256; ++i)
        p[i] = saturate_cast<uchar>(pow(i / 255.0, 2) * 255.0);
    Mat result = oneChannelImage.clone();
    LUT(oneChannelImage, lookUpTable, result);

    return result;
}
