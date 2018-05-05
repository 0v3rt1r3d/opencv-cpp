#ifndef GRUZINTSEV_A_S_NIGHTRAINDETECTOR_H
#define GRUZINTSEV_A_S_NIGHTRAINDETECTOR_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <algorithm>
#include <functional>

class NightRainDetector {

private:
    static constexpr float NIGHT_UP_THRESHOLD = 0.1;
    static constexpr float NIGHT_TOP_PART = 0.3;

    static constexpr float SCALE_KOEF = 0.6;

    static constexpr int BRIGHTNESS_RANGE_TOP = 250;
    static constexpr int SATURATION_RANGE_BOTTOM = 50;
    static constexpr int SATURATION_RANGE_TOP = 200;

    static constexpr int HUE_RANGE_TOP = 170;
    static constexpr int HUE_RANGE_BOTTOM = 20;

    static constexpr float ADDITIONAL_WIDTH = 0.04;


public:
    bool detect(std::string path, bool showPictures);

private:
    std::vector<cv::Mat> splitToChannels(cv::Mat &mat);

    std::vector<cv::Rect> findLights(cv::Mat &mat);

    std::vector<cv::Rect> blinksBelowRects(cv::Mat &oneChannelMat, std::vector<cv::Rect> &rects);

    void showHist(cv::Mat oneChannel);

    bool isNight(cv::Mat &mat);

    cv::Mat correctBrightness(cv::Mat brighnessChannel);

private:
    cv::Mat apply(cv::Mat &mat, std::function<void(cv::Mat &, cv::Mat &)> lambda) {
        cv::Mat processed;
        lambda(mat, processed);
        return processed;
    }
};


#endif //GRUZINTSEV_A_S_NIGHTRAINDETECTOR_H
