#include <iostream>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

int main()
{
    Mat m(800, 800, CV_8UC3);

    for (int i = 0; i < m.cols; i++) {
        for (int j = 0; j < m.rows; j++) {
            m.at<Vec3b>(i, j)[0] = sin(i * 0.01) * 128 + 128;
            m.at<Vec3b>(i, j)[1] = sin(j * 0.01) * 128 + 128;
            m.at<Vec3b>(i, j)[2] = (sin(i * 0.01) + sin(j * 0.01)) * 128 + 128;
        }
    }

    imshow("SINPIC", m);
    waitKey(0);
    imwrite("/home/overtired/Desktop/pictures/picture_waves.png", m);
    
    return 0;
}
