#include "motionProfiling.h"

#include <math.h>
#include <memory>

#include "robotConstants.h"

// https://www.desmos.com/calculator/avvhmhcaec
double velFunction(double x, double d, double t) {
    return (d/2)*(cos(x*(M_PI/t)-(M_PI/2))*(M_PI/t));
}

// Make sure to de-allocate vector when done
motionProfiling::Profile motionProfiling::genVelProfile(double dist) {
    double time = motionProfiling::baseMoveTime;
    

    while (true) {
        if (velFunction(time/2, dist, time) > motionProfiling::maxVel) {
            time += timeIncrement;
            continue;
        }

        DEBUGLOG("Max Vel: ", velFunction(time/2, dist, time));
        DEBUGLOG("Time: ", time);

        // Generate the curve
        motionProfiling::Profile curve(new std::vector<double>);

        for (double i = 0.00; i <= time; i+= timeIncrement) {
            curve->push_back(velFunction(i, dist, time));
        }

        return curve;
    }

    return nullptr;
}