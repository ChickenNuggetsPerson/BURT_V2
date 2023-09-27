#pragma once

using namespace vex;

static vision::signature SIG_1 (1, -9363, -7741, -8552, -7835, -6187, -7011, 6.000, 0);

static vex::vision visionSensor(vex::PORT8, 50, SIG_1);