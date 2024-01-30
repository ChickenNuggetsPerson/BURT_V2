#include "motionProfiling.h"

#include <math.h>
#include <memory>

#include "robotConstants.h"
#include "robotConfig.h"

// https://www.desmos.com/calculator/koohegr4hh
double velFunction(double x, double d, double t) {
    //return (d/2)*(cos(x*(M_PI/t)-(M_PI/2))*(M_PI/t)); Old Velocity Function

    double top = 2 * pow(M_E, (log(0.01/d) * ((2 * x) - t)) / t) * d * log(0.01/d);
    double bottom = t * pow(1 + pow(M_E, (log(0.01/d) * ((2 * x) - t)) / t), 2);

    return -(top/bottom);
}

// Make sure to de-allocate vector when done
motionProfiling::Profile motionProfiling::genVelProfile(double dist) {
    double time = motionProfiling::accelerationTime;

    motionProfiling::Profile curve(new std::vector<double>);
    // Total acceleration and deceleration distance
    double baseDist = accelerationDist; 
    
    double distTraveled = baseDist;
    double scaleDown = 1;


    // Add acceleration curve
    for (double i = 0.00; i <= time/2; i += timeIncrement) {
        curve->push_back(velFunction(i, baseDist, time) * scaleDown);
    }

    // Integrate up to the max
    double maxVel = velFunction(time/2, baseDist, time)  * scaleDown;
    while (dist > distTraveled) {
        curve->push_back(maxVel);
        distTraveled += maxVel * timeIncrement;
    }

    // Add deceleration curve
    for (double i = time/2; i <= time; i += timeIncrement) {
        curve->push_back(velFunction(i, baseDist, time)  * scaleDown);
    }

    return curve;

}