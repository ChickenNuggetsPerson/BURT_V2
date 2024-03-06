#include "pid.h"


using namespace pid;


PID::PID(PIDConfig config) {
    P = config.P;
    I = config.I;
    D = config.D;
};
PID::PID(PIDConfig config, double desired) {
    P = config.P;
    I = config.I;
    D = config.D;
    desiredVal = desired;
};

void PID::changeConfig(PIDConfig newConfig) {
    P = newConfig.P;
    I = newConfig.I;
    D = newConfig.D;
}



void PID::setMax(double maxVal) {max = maxVal; hasMax = true;}
void PID::setMin(double minVal) {min = minVal; hasMin = true;}

double PID::iterate(double newVal, double newDesired) {
    desiredVal = newDesired;
    return iterate(newVal);
};

// Credit to Connor 1814D on youtube for this simple yet concise PID code bellow
// https://www.youtube.com/watch?v=_Itn-0d340g&ab_channel=Connor1814D
// I adapted it into a class object that can be easily created and used
double PID::iterate(double newVal) {

    error = newVal - desiredVal;

    if (isTimeScaling) {
        double now = brainPtr->timer(timerUnits);
        dt = now - lastTime;        

        derivative = (error - prevError) / dt;
        totalError += error * dt;

        lastTime = now;

    } else {
        derivative = error - prevError;
        totalError += error;
    }
    
    if (hasIntegralLimit) {

        if (totalError * I > maxI) {
            totalError = maxI / I;
        }
        if (totalError * I < minI) {
            totalError = minI / I;
        }

    }


    double result = (error * P) + (derivative * D) + (totalError * I);

    prevError = error;

    if (hasMin) {if (result < min) { result = min; }} // Limit the PID to defined min and max vales
    if (hasMax) {if (result > max) { result = max; }}

    return result;
};