#pragma once

#include <vector>
#include <memory>

namespace motionProfiling {

    using Profile = std::unique_ptr<std::vector<double>>;

    static double baseMoveTime = 0.5;
    static double maxVel = 12*2;
    static double timeIncrement = 0.1;

    Profile genVelProfile(double dist);

}
