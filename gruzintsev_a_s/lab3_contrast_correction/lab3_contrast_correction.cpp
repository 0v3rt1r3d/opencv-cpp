#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

int main()
{
    Mat image = imread("../pictures/girl.png");

    std::cout << "channels = " << image.channels() << std::endl;

    Mat hist;

    int channels[] = {0};

    float range_0[] = {0, 255};
    const float* ranges[] = { range_0 };

    int hbins = 255;
    int histSize[] = {hbins};

    calcHist(&image, 1, channels, Mat(), hist, 1, histSize, ranges);

    bool left = false;
    bool right = false;

    int quantLeft, quantRight;
    float sumLeft = 0, sumRight = 0;

    for (int i = 0; i < hist.size[0]; i++) {

        if (!left && sumLeft / (image.size[0]*image.size[1]) < 0.1) {
            sumLeft += hist.at<float>(i);
            quantLeft = i;
        } else {
            left = true;

        }

        if (!right && sumRight / (image.size[0]*image.size[1]) < 0.1) {
            sumRight += hist.at<float>(hist.size[0] - 1 - i);
            quantRight = hist.size[0] - 1 - i;
        } else {
            right = true;
        }
    }

    uchar table[256];

    for (int i = 0; i < 256; ++i) {
        if (i < quantLeft) {
            table[i] = (uchar)0;
        } else if (i > quantRight) {
            table[i] = (uchar)255;
        } else {
            table[i] = (uchar)(255.0 / (quantRight - quantLeft)  * (i - quantLeft));
        }
    }

    Mat target(image.rows, image.cols, CV_8UC1);

    for (int i = 0; i < target.rows; i++) {
        for (int j = 0; j < target.cols; j++) {
            target.at<uchar>(i, j) = table[image.at<Vec3b>(i, j)[1]];
        }
    }


    imshow("Source", image);
    imshow("Processed", target);

    waitKey(0);

    return 0;
}
