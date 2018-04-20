#ifndef GRUZINTSEV_A_S_NIGHTRAINDETECTORTESTER_H
#define GRUZINTSEV_A_S_NIGHTRAINDETECTORTESTER_H

#include <iostream>
#include "NightRainDetector.h"

class NightRainDetectorTester {
private:
    static const int SPACE_BAR = 32;

public:
    void testByGlob(std::string &pathGlob);

private:
    void test(std::set<std::string> &paths);
};


#endif //GRUZINTSEV_A_S_NIGHTRAINDETECTORTESTER_H
