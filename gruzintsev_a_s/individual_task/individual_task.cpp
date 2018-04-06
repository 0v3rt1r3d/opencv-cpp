#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Mat detecRain(string &path) {
    Mat raw = imread(path, IMREAD_COLOR);
    Mat source = imread(path, IMREAD_COLOR);
//    resize(raw,source,Size(raw.cols/2,raw.rows/2));


    Mat result(source.rows, source.cols, CV_8UC3);

    source.copyTo(result(Rect(0, 0, source.cols, source.rows)));

    Mat sourceInHSV;
    cvtColor(source, sourceInHSV, CV_BGR2HSV);

    vector<Mat> channels;
    split(sourceInHSV, channels);

    Mat filtredV = channels[2] > 220;
//    filtredV.copyTo(result(Rect(source.cols, 0, source.cols, source.rows)));

    imshow("FILTRED", filtredV);

    return result;
}

int main() {
    string pathToImage = "../gruzintsev_a_s/individual_task/night-rain/000002.jpg";
    Mat detected = detecRain(pathToImage);

    imshow("RESULT", detected);

    waitKey(0);
    return 0;
}
