#include "NightRainDetectorTester.h"

using namespace std;

int main() {
    NightRainDetectorTester tester;

    string pathToNight = "../gruzintsev_a_s/individual_task/night-rain/*.jpg";
    string pathToOther = "../gruzintsev_a_s/individual_task/other/*.jpg";

    tester.testByGlob(pathToNight);
    tester.testByGlob(pathToOther);
//    transform()

    return EXIT_SUCCESS;
}
