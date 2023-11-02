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
    double time = motionProfiling::baseMoveTime;

    motionProfiling::Profile curve(new std::vector<double>);
    double baseDist = 12; // Inches
    double scaleDown = 0.97;

    while (true) {
        if (velFunction(time/2, baseDist, time) > motionProfiling::maxVel) {
            time += timeIncrement;
            continue;
        }
        // Add to the curve
        for (double i = 0.00; i <= time; i+= timeIncrement) {
            curve->push_back(velFunction(i, baseDist, time));
        }
        break;
    }

    int length = curve.get()->size();
    double middleSpeed = curve.get()->at(length/2);

    motionProfiling::Profile finalCurve(new std::vector<double>);
    // Add the first half of the curve
    for (int i = 0; i < length / 2; i++) {
        finalCurve.get()->push_back(curve.get()->at(i) * scaleDown);
    }

    while (baseDist < dist) {
        double deltaDist = middleSpeed * timeIncrement;
        finalCurve.get()->push_back(middleSpeed * scaleDown);
        baseDist += deltaDist;
    }

    for (int i = length / 2; i < length; i++) {
        finalCurve.get()->push_back(curve.get()->at(i) * scaleDown);
    }

    return finalCurve;
}