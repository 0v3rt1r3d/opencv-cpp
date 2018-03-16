#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

int main()
{
    Mat m = imread("../pictures/27816.jpg", COLOR_BGR2HLS);
    vector<Mat> channels;
    split(m, channels);
    
    Mat result1, result2;
    Mat mask1 = channels[0] > 150;
    Mat mask2 = channels[1] > 150;

    m.copyTo(result1, mask1);
    m.copyTo(result2, mask2);

    Mat twoImages(m.rows, m.cols * 3, CV_8UC3);
    m.copyTo(twoImages(Rect(0, 0, m.cols, m.rows)));
    result1.copyTo(twoImages(Rect(m.cols, 0, m.cols, m.rows)));
    result2.copyTo(twoImages(Rect(2 * m.cols, 0, m.cols, m.rows)));

    imshow("WINDOW", twoImages);
    waitKey(0);

    return 0;
}
