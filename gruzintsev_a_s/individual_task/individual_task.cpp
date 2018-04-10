#include <iostream>
#include <opencv2/opencv.hpp>
#include <Poco/Glob.h>

using namespace cv;
using namespace std;

Mat detecRain(string &path) {
    Mat raw = imread(path, IMREAD_COLOR);
    Mat source = imread(path, IMREAD_COLOR);
    resize(raw, source, Size(raw.cols / 2, raw.rows / 2));


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

            Mat detected = detecRain(path);
            imshow("RESULT", detected);

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
