#include <iostream>
#include <cmath>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main() {
    Mat m(500, 500, CV_8UC3);

    for (int i = 0; i < m.cols; i++) {
        for (int j = 0; j < m.rows; j++) {
            m.at<Vec3b>(i, j)[0] = (uchar) (sin(i * 0.02) * 128 + 128);
            m.at<Vec3b>(i, j)[1] = (uchar) (sin(j * 0.02) * 128 + 128);
            m.at<Vec3b>(i, j)[2] = (uchar) ((sin(i * 0.02) + sin(j * 0.02)) * 128 + 128);
        }
    }

    imwrite("../pictures/picture_waves.png", m);

    imshow("Lab 1 Sinusoudal circles", m);

    waitKey(0);

    return 0;
}
