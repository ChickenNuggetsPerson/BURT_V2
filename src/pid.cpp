#include "pid.h"


PIDConfig::PIDConfig() {
    P = 0.00;
    I = 0.00;
    D = 0.00;
};
PIDConfig::PIDConfig(double p, double i, double d) {
    P = p;
    I = i;
    D = d;
};


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
    derivative = error - prevError;
    totalError += error;
    
    double result = error * P + derivative * D + totalError * I;

    prevError = error;

    if (hasMin) {if (result < min) { result = min; }} // Limit the PID to defined min and max vales
    if (hasMax) {if (result > max) { result = max; }}

    return result;
};