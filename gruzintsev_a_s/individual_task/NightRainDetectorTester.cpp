#include "NightRainDetectorTester.h"
#include <Poco/Glob.h>

using namespace std;
using namespace cv;


void NightRainDetectorTester::test(set<string> &paths) {
    NightRainDetector detector;

    for (int i = 0; i < paths.size(); i++) {
        set<string>::iterator it = paths.begin();
        advance(it, i);
        auto path = *it;

        cout << "[PATH:] " << path;

        detector.detect(path);

//        while (true) {
//            if (cv::waitKey(0) == SPACE_BAR) {
//                break;
//            }
//        }
    }
}

void NightRainDetectorTester::testByGlob(string &pathGlob) {
    set<string> paths;
    Poco::Glob::glob(pathGlob, paths);
    test(paths);
}
