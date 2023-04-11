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

void PID::setMax(double maxVal) {max = maxVal; hasMax = true;}
void PID::setMin(double minVal) {min = minVal; hasMin = true;}

double PID::iterate(double newVal, double newDesired) {
    desiredVal = newDesired;
    return iterate(newVal);
};
double PID::iterate(double newVal) {

    error = newVal - desiredVal;
    derivative = error - prevError;
    totalError += error;

    double result = error * P + derivative * D + totalError * I;

    prevError = error;

    if (hasMin) {if (result < min) { result = min; }}
    if (hasMax) {if (result > max) { result = max; }}

    return result;
};