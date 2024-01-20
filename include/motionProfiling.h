#pragma once

#include <vector>
#include <memory>

namespace motionProfiling {

    using Profile = std::unique_ptr<std::vector<double>>;

    static double baseMoveTime = 0.2;
    static double maxVel = 72;
    static double timeIncrement = 0.1;

    Profile genVelProfile(double dist);

}
