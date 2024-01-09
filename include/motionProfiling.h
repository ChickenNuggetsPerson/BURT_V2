#pragma once

#include <vector>
#include <memory>

namespace motionProfiling {

    using Profile = std::unique_ptr<std::vector<double>>;

    static double baseMoveTime = 1;
    static double maxVel = 36;
    static double timeIncrement = 0.1;

    Profile genVelProfile(double dist);

}
