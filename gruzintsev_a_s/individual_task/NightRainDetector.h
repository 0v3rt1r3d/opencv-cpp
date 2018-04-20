#ifndef GRUZINTSEV_A_S_NIGHTRAINDETECTOR_H
#define GRUZINTSEV_A_S_NIGHTRAINDETECTOR_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <algorithm>
#include <functional>

class NightRainDetector {

private:
    static constexpr double NIGHT_UP_THRESHOLD = 0.3;
    static constexpr double NIGHT_TOP_PART = 0.3;

    static constexpr float SCALE_KOEF = 0.6;

    static constexpr int BRIGHTNESS_RANGE_TOP = 250;
    static constexpr int SATURATION_RANGE_BOTTOM = 50;
    static constexpr int SATURATION_RANGE_TOP = 200;

    static constexpr int HUE_RANGE_TOP = 170;
    static constexpr int HUE_RANGE_BOTTOM = 20;


public:
    void detect(std::string path);

private:
    void filterWithCustomKernel(cv::Mat &mat);

    cv::Mat horizontalDerivative(cv::Mat &mat);

    cv::Mat verticalDerivative(cv::Mat &mat);

    cv::Mat horizontalDerivativeKernel();

    cv::Mat customKernel();

    cv::Mat applyGammaCorrection(cv::Mat& source);

    std::vector<cv::Mat> splitToChannels(cv::Mat& mat);

    void showChannels(std::vector<cv::Mat> &channels);

    bool isNight(cv::Mat &mat);

private:
    cv::Mat apply(cv::Mat &mat, std::function<void(cv::Mat&, cv::Mat&)> lambda) {
        cv::Mat processed;
        lambda(mat, processed);
        return processed;
    }
};


#endif //GRUZINTSEV_A_S_NIGHTRAINDETECTOR_H
