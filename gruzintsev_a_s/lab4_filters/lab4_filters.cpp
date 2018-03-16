#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main() {
    Mat sourceImage = imread("../pictures/camaro.png", CV_LOAD_IMAGE_GRAYSCALE);

    Mat linearFilterExample;
    Mat boxFilterExample;
    Mat medianBlurExample;

    Point anchor = Point(0, 0);
    double delta = 0;
    int depth = -1;

    Mat kernel = Mat(1, 2, CV_32F);

    kernel.at<float>(0, 0) = -0.5f;
    kernel.at<float>(0, 1) = 0.5f;

    filter2D(sourceImage, linearFilterExample, depth, kernel, anchor, delta, BORDER_DEFAULT);
    boxFilter(sourceImage, boxFilterExample, 0, Size(3, 3));
    medianBlur(sourceImage, medianBlurExample, 9);

    Mat combinedImages(sourceImage.rows * 2, sourceImage.cols * 2, CV_8UC1);

    sourceImage.copyTo(combinedImages(Rect(0, 0, sourceImage.cols, sourceImage.rows)));
    boxFilterExample.copyTo(combinedImages(Rect(sourceImage.cols, 0, sourceImage.cols, sourceImage.rows)));
    linearFilterExample.copyTo(combinedImages(Rect(0, sourceImage.rows - 1, sourceImage.cols, sourceImage.rows)));
    medianBlurExample.copyTo(
            combinedImages(Rect(sourceImage.cols, sourceImage.rows, sourceImage.cols, sourceImage.rows)));

    imshow("Lab 4 Filters", combinedImages);

    waitKey(0);

    return 0;
}
