#include <iostream>
#include <opencv2/opencv.hpp>
#include <Poco/Glob.h>

using namespace cv;
using namespace std;

class NightRainPicturesDetector {
private:
    static constexpr double NIGHT_UP_THRESHOLD = 0.3;
    static constexpr double NIGHT_TOP_PART = 0.3;
    static constexpr float SCALE_KOEF = 0.6;
    static constexpr int BRIGHTNESS_RANGE_TOP = 220;
    static constexpr int SATURATION_RANGE_BOTTOM = 50;
    static constexpr int SATURATION_RANGE_TOP = 200;

    static constexpr int HUE_RANGE_TOP = 170;
    static constexpr int HUE_RANGE_BOTTOM = 20;


public:
    static void detect(string path, bool debugMode) {
        Mat raw = imread(path, IMREAD_COLOR);
        Mat source = imread(path, IMREAD_COLOR);
        resize(raw, source, Size(int(raw.cols * SCALE_KOEF), int(raw.rows * SCALE_KOEF)));

        Mat result(source.rows, source.cols, CV_8UC3);

        source.copyTo(result(Rect(0, 0, source.cols, source.rows)));

        Mat sourceInHSV;
        cvtColor(source, sourceInHSV, CV_BGR2HSV);

        showChannels(sourceInHSV);

        vector<Mat> channels;
        split(sourceInHSV, channels);

        Mat bluredImage;

//        cout << isNight(channels[2]) << endl;

//        filterWithCustomKernel(channels[2]);
        filterChannels(sourceInHSV);

        imshow("RESULT", result);
    }

private:
    static void filterWithCustomKernel(Mat &mat) {
        Mat filtered1;
        Mat filtered2;
        int SAME_DEPTH = -1;

        Mat kernel = customKernel();
        Point center = Point(-1, -1);

        filter2D(mat, filtered1, SAME_DEPTH, customKernel(), center);
//        filter2D(mat, filtered2, SAME_DEPTH, customKernel(), center);

        imshow("Custom filter", filtered1);
    }

    static Mat horizontalDerivative(Mat &mat) {
        Mat result(mat.rows, mat.cols * 3, CV_8UC1);
        vector<Mat> channels;
        split(mat, channels);

        Mat kernel = horizontalDerivativeKernel();
        Mat derivative;

        int SAME_DEPTH = -1;
        Point center = Point(-1, -1);

        Mat blured;
        medianBlur(channels[2], blured, 7);

        filter2D(blured, derivative, SAME_DEPTH, kernel, center);

        Mat normalizedDerivative;
        Mat derivativeSquare;
        pow(derivative, 2, derivativeSquare);
        derivativeSquare.convertTo(normalizedDerivative, CV_8UC1);

        return normalizedDerivative;
    }


    static void filterChannels(Mat &mat) {
//        Mat result(mat.rows, mat.cols * 3, CV_8UC1);

        vector<Mat> channels;
        split(mat, channels);


        Mat ranged1 = channels[2] > BRIGHTNESS_RANGE_TOP;
        Mat ranged2 = channels[1] < SATURATION_RANGE_BOTTOM | channels[1] > SATURATION_RANGE_TOP;
//        Mat ranged3 = channels[0] < HUE_RANGE_BOTTOM | channels[0] > HUE_RANGE_TOP;

//        ranged1.copyTo(result(Rect(0, 0, mat.cols, mat.rows)));
//        ranged2.copyTo(result(Rect(mat.cols, 0, mat.cols, mat.rows)));
//        ranged3.copyTo(result(Rect(2 * mat.cols, 0, mat.cols, mat.rows)));
//
//        line(result, Point(mat.cols, 0), Point(mat.cols, mat.rows), Scalar(255), 4);

        Mat derivative = horizontalDerivative(mat);

        Mat combined = ranged2 & derivative;

        imshow("Derivative", derivative);
        imshow("Combined", combined);
    }

    static Mat horizontalDerivativeKernel() {
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

    static Mat customKernel() {
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

    static void showChannels(Mat &mat) {
        Mat result(mat.rows, mat.cols * 3, CV_8UC1);
        vector<Mat> channels;
        split(mat, channels);

        for (int i = 0; i < 3; i++) {
            channels[i].copyTo(result(Rect(mat.cols * i, 0, mat.cols, mat.rows)));
        }

        imshow("Channels", result);
    }

    static bool isNight(Mat &mat) {
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
};

class MethodTester {
public:
    static void testOnNightRain() {
        auto paths = nightRainPaths();
        test(paths);
    }

    static void testOnOther() {
        auto paths = otherPaths();
        test(paths);
    }

private:
    static const int SPACE_BAR = 32;

    static set<string> nightRainPaths() {
        set<string> paths;
        Poco::Glob::glob("../gruzintsev_a_s/individual_task/night-rain/*.jpg", paths);
        return paths;
    }

    static set<string> otherPaths() {
        set<string> paths;
        Poco::Glob::glob("../gruzintsev_a_s/individual_task/other/*.jpg", paths);
        return paths;
    }

private:
    static void test(set<string> &paths) {
        for (int i = 0; i < paths.size(); i++) {
            set<string>::iterator it = paths.begin();
            advance(it, i);
            string path = *it;

            cout << "[PATH:] " << path << endl;

            NightRainPicturesDetector::detect(path, true);

            while (true) {
                if (waitKey(0) == SPACE_BAR) {
                    break;
                }
            }
        }
    }
};


int main() {
    MethodTester::testOnNightRain();
    MethodTester::testOnOther();

    return 0;
}
