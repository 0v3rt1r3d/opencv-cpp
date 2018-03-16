#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main() {
    Mat sourceImage = imread("../pictures/camaro.png");
    Mat sourceInHSV;
    cvtColor(sourceImage, sourceInHSV, CV_BGR2HSV);

    vector<Mat> channels;
    split(sourceInHSV, channels);

    Mat rangedHue, rangedSaturation;
    Mat maskHue = channels[0] < 30 | channels[0] > 120; // Hue range is between 0 and 180 in openCV
    Mat maskSaturation = channels[1] > 80;

    sourceInHSV.copyTo(rangedHue, maskHue);
    sourceInHSV.copyTo(rangedSaturation, maskSaturation);

    Mat combinedImagesInHSV(sourceImage.rows, sourceImage.cols * 3, CV_8UC3);
    sourceInHSV.copyTo(combinedImagesInHSV(Rect(0, 0, sourceImage.cols, sourceImage.rows)));
    rangedHue.copyTo(combinedImagesInHSV(Rect(sourceImage.cols, 0, sourceImage.cols, sourceImage.rows)));
    rangedSaturation.copyTo(combinedImagesInHSV(Rect(2 * sourceImage.cols, 0, sourceImage.cols, sourceImage.rows)));

    Mat combinedImages;
    cvtColor(combinedImagesInHSV, combinedImages, CV_HSV2BGR);

    imshow("Lab 2 Brightness range", combinedImages);
    waitKey(0);

    return 0;
}
