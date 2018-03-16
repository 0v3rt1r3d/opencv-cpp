#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

int main()
{
    Mat image = imread("../pictures/girl.png", CV_BGR2GRAY);

    Mat destinationImage;
    Mat box;
    Mat median;
    Mat src;
    image(Rect(Point(100, 0), Point(600, 400))).copyTo(src);
    src.copyTo(destinationImage);
    src.copyTo(box);
    src.copyTo(median);

    /// Initialize arguments for the filter
    Point anchor = Point( 0, 0 );
    double delta = 0;
    int ddepth = -1;

    /// Loop - Will filter the image with different kernel sizes each 0.5 seconds
    int ind = 0;

    /// Update kernel size for a normalized box filter
    Mat kernel = Mat::ones( 1, 2, CV_32F );

    kernel.at<float>(0, 0) = -0.5;
    kernel.at<float>(0, 1) = 0.5;

    filter2D(destinationImage, destinationImage, ddepth, kernel, anchor, delta, BORDER_DEFAULT );
    boxFilter(src, box, 0, Size(3, 3));
    medianBlur(src, median, 9);


    imshow("Source", src);
    imshow("Processed", destinationImage);
    imshow("Box", box);
    imshow("Median", median);

    waitKey(0);

    return 0;
}
