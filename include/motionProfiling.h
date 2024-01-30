#pragma once

#include <vector>
#include <memory>

namespace motionProfiling {

    using Profile = std::unique_ptr<std::vector<double>>;

    // Adjust vel function in desmos to get correct max speed
    static double accelerationTime = 0.5; // Results in max speed of 48in/sec
    static double accelerationDist = 6.0;

    static double timeIncrement = 0.1;

    Profile genVelProfile(double dist);

}
