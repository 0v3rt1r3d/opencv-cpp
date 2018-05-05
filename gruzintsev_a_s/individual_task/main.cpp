#include "NightRainDetectorTester.h"

using namespace std;

int main() {
    NightRainDetectorTester tester;

    string pathToNight = "../gruzintsev_a_s/individual_task/night-rain/*.jpg";
    string pathToOther = "../gruzintsev_a_s/individual_task/other/*.jpg";
    string pathToTest = "../gruzintsev_a_s/individual_task/test/*.jpg";

    tester.testByGlob(pathToNight, false);
//    tester.testByGlob(str, true);
    tester.testByGlob(pathToOther, false);
//    tester.testByGlob(pathToTest, false);
//    tester.testByGlob(pathToTest, true);

    return EXIT_SUCCESS;
}
