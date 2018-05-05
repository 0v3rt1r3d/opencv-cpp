#include "NightRainDetectorTester.h"

using namespace std;

int main() {
    NightRainDetectorTester tester;

    string pathToNight = "../gruzintsev_a_s/individual_task/night-rain/*.jpg";
    string pathToOther = "../gruzintsev_a_s/individual_task/other/*.jpg";

    tester.testByGlob(pathToNight, true);
    tester.testByGlob(pathToOther, true);

    return EXIT_SUCCESS;
}
