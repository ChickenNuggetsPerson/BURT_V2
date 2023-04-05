
#include "auton.h"
#include "robotConfig.h"

using namespace vex;

void ai::init() {

    brainFancyDebug("Auton Initialized ", cyan);
};

void ai::started() {
    brainFancyDebug("Auton Started", vex::color::cyan);
    brainChangePage("auton");
    brainPageChangeText("auton", "description", 0, "this is the description");
};