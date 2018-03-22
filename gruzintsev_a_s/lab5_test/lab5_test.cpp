#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void drawCircle(Mat &mat, const Scalar &color) {
    circle(mat, Point(mat.cols / 2, mat.rows / 2), int(min(mat.rows, mat.cols) / 2 * 0.7), color, CV_FILLED);
}

void drawRect(Mat &mat, const Scalar &color) {
    rectangle(mat, Point(int(mat.cols / 2 * 0.5), int(mat.rows / 2 * 0.5)),
              Point(int(mat.cols / 2 * 1.5), int(mat.rows / 2 * 1.5)), color, CV_FILLED);
}

void drawTriangle(Mat &mat, const Scalar &color) {
    Point a = Point(mat.cols / 2, mat.rows / 5);
    Point b = Point(mat.cols / 5, mat.rows * 4 / 5);
    Point c = Point(mat.cols * 4 / 5, mat.rows * 4 / 5);

    int count_of_polygons = 1;

    Point points_in_one_polygon[3] = {a, b, c};
    const Point *polygon[count_of_polygons] = {points_in_one_polygon};

    int number_of_points_in_polygons[] = {3};

    fillPoly(mat, polygon, number_of_points_in_polygons, count_of_polygons, color);
}

void drawBackground(Mat &mat, const Scalar &color) {
    Mat background(mat.cols, mat.rows, CV_8UC1, color);
    background.copyTo(mat);
}

Mat getPictureWithDifferentFigures() {
    const int PICTURE_WIDTH = 100;

    uchar black = 0;
    uchar gray = 127;
    uchar white = 255;

    uchar colors0[] = {black, gray, white};
    uchar colors1[] = {gray, white, black};
    uchar colors2[] = {white, black, gray};

    Mat combinedImage(PICTURE_WIDTH * 3, PICTURE_WIDTH * 6, CV_8UC1);
    Mat area;

    int w_step = 0;
    int h_step = 0;

    for (int i_color = 0; i_color < 3; i_color++) {
        area = combinedImage(Rect(w_step, h_step, PICTURE_WIDTH, PICTURE_WIDTH));
        drawBackground(area, colors0[i_color]);
        drawCircle(area, colors1[i_color]);
        w_step += PICTURE_WIDTH;
    }

    for (int i_color = 0; i_color < 3; i_color++) {
        area = combinedImage(Rect(w_step, h_step, PICTURE_WIDTH, PICTURE_WIDTH));
        drawBackground(area, colors0[i_color]);
        drawCircle(area, colors2[i_color]);
        w_step += PICTURE_WIDTH;
    }

    w_step = 0;
    h_step += PICTURE_WIDTH;

    for (int i_color = 0; i_color < 3; i_color++) {
        area = combinedImage(Rect(w_step, h_step, PICTURE_WIDTH, PICTURE_WIDTH));
        drawBackground(area, colors1[i_color]);
        drawRect(area, colors2[i_color]);
        w_step += PICTURE_WIDTH;
    }

    for (int i_color = 0; i_color < 3; i_color++) {
        area = combinedImage(Rect(w_step, h_step, PICTURE_WIDTH, PICTURE_WIDTH));
        drawBackground(area, colors1[i_color]);
        drawRect(area, colors0[i_color]);
        w_step += PICTURE_WIDTH;
    }

    w_step = 0;
    h_step += PICTURE_WIDTH;

    for (int i_color = 0; i_color < 3; i_color++) {
        area = combinedImage(Rect(w_step, h_step, PICTURE_WIDTH, PICTURE_WIDTH));
        drawBackground(area, colors2[i_color]);
        drawTriangle(area, colors0[i_color]);
        w_step += PICTURE_WIDTH;
    }

    for (int i_color = 0; i_color < 3; i_color++) {
        area = combinedImage(Rect(w_step, h_step, PICTURE_WIDTH, PICTURE_WIDTH));
        drawBackground(area, colors2[i_color]);
        drawTriangle(area, colors1[i_color]);
        w_step += PICTURE_WIDTH;
    }

    return combinedImage;
}

Mat getKernel() {
    Mat kernel(3, 3, CV_32S);

    kernel.at<int>(0, 0) = 1;
    kernel.at<int>(1, 0) = 1;
    kernel.at<int>(2, 0) = 1;

    kernel.at<int>(0, 1) = 0;
    kernel.at<int>(1, 1) = 0;
    kernel.at<int>(2, 1) = 0;

    kernel.at<int>(0, 2) = -1;
    kernel.at<int>(1, 2) = -1;
    kernel.at<int>(2, 2) = -1;

    return kernel;
}

Mat flip(const Mat &mat) {
    Mat flipped;
    flip(mat, flipped, 1);
    return flipped;
}

Mat filter(const Mat &mat, const Mat &kernel) {
    Mat result;
    filter2D(mat, result, -1, kernel);
    return result;
}

Mat merge(const Mat &mat1, const Mat &mat2, const Mat &mat3) {
    Mat result;
    Mat layers[] = {mat1, mat2, mat3};
    merge(layers, 3, result);

    return result;
}

Mat convert(const Mat &mat, int cv_type) {
    Mat converted;
    cvtColor(mat, converted, cv_type);
    return converted;
}

int main() {
    Mat image = getPictureWithDifferentFigures();
    Mat allDerivatives(image.rows * 2, image.cols * 2, image.type());

    imshow("Source image", image);

    Mat kernel = getKernel();

    Mat derivativeLeft = filter(image, kernel);
    Mat derivativeRight = filter(image, flip(kernel));
    Mat derivativeUp = filter(image, kernel.t());
    Mat derivativeDown = filter(image, flip(kernel).t());

    derivativeLeft.copyTo(allDerivatives(Rect(0, 0, image.cols, image.rows)));
    derivativeUp.copyTo(allDerivatives(Rect(image.cols, 0, image.cols, image.rows)));
    derivativeRight.copyTo(allDerivatives(Rect(0, image.rows, image.cols, image.rows)));
    derivativeDown.copyTo(allDerivatives(Rect(image.cols, image.rows, image.cols, image.rows)));

    Mat absoluteGradient = (derivativeUp + derivativeDown + derivativeLeft + derivativeRight) / 4;
    Mat result = convert(merge(derivativeUp, derivativeRight, absoluteGradient), CV_HSV2BGR);

    imshow("Derivatives", allDerivatives);
    imshow("Gradient", absoluteGradient);
    imshow("Result", result);

    waitKey(0);

    return 0;
}
