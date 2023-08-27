#pragma once

using namespace vex;

vision::signature SIG_1 (1, -9363, -7741, -8552, -7835, -6187, -7011, 6.000, 0);
vision::signature SIG_2 (2, 0, 0, 0, 0, 0, 0, 8.000, 0);
vision::signature SIG_3 (3, 0, 0, 0, 0, 0, 0, 4.600, 0);
vision::signature SIG_4 (4, 0, 0, 0, 0, 0, 0, 3.000, 0);
vision::signature SIG_5 (5, 0, 0, 0, 0, 0, 0, 3.000, 0);
vision::signature SIG_6 (6, 0, 0, 0, 0, 0, 0, 3.000, 0);
vision::signature SIG_7 (7, 0, 0, 0, 0, 0, 0, 3.000, 0);

extern vex::vision visionSensor(vex::PORT18, 50, SIG_1, SIG_2, SIG_3, SIG_4, SIG_5, SIG_6, SIG_7 );