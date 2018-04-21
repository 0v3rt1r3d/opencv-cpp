#include "NightRainDetectorTester.h"
#include <Poco/Glob.h>

using namespace std;
using namespace cv;


void NightRainDetectorTester::test(set<string> &paths, bool makeBreaks) {
    NightRainDetector detector;

    int deteced = 0;
    int all = 0;

    for (int i = 0; i < paths.size(); i++) {
        all += 1;
        set<string>::iterator it = paths.begin();
        advance(it, i);
        auto path = *it;

        cout << "[PATH:] " << path;

        if (detector.detect(path, makeBreaks)) {
            deteced += 1;
        }

        if (makeBreaks) {
            while (true) {
                if (cv::waitKey(0) == SPACE_BAR) {
                    break;
                }
            }
        }
    }

    cout << "[TOTAL:] " << deteced << " / " << all << endl;
}

void NightRainDetectorTester::testByGlob(string &pathGlob, bool makeBreaks) {
    set<string> paths;
    Poco::Glob::glob(pathGlob, paths);
    test(paths, makeBreaks);
}
