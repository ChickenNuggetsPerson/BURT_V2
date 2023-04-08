#include "robotConfig.h"
#include "odometry.h"

using namespace vex;

using std::cout;
using std::endl;

// Main system for tracking the position of the robot

int mainTrackingTask(void* system) {
    OdometrySystem* systemPointer = (OdometrySystem*)system;

    double updateSpeed = 100; // In msec 

    wait(0.5, seconds);

    // V-- Put back in once inertial sensor is installed
    //if (!inertialSensor.installed()) { brainError("Inertial Sensor Not Installed"); return 1;}

    while (inertialSensor.isCalibrating()) {
        wait(0.05, seconds);
    }
    
    brainFancyDebug("Starting Odometry System", green, true);
    systemPointer->isTracking = true;
    while (true) {
        systemPointer->track();
        wait(updateSpeed, msec);
    }
    return 1;
}

OdometrySystem::OdometrySystem() {
    // Start tracking system
    //trackingTask = vex::task(mainTrackingTask, (void*)this, vex::task::taskPriorityNormal);
}

void OdometrySystem::restart(Position currentPos) {
    if (isTracking) { trackingTask.stop(); isTracking = false; }
    wait(0.1, sec);
    xPos = currentPos.x;
    yPos = currentPos.y;
    rot = currentPos.rot;
    wait(0.1, sec);
    trackingTask = vex::task(mainTrackingTask, (void*)this, vex::task::taskPriorityNormal);
};

Position OdometrySystem::currentPos() {
    Position tempPos;
    tempPos.x = xPos;
    tempPos.y = yPos;
    tempPos.rot = rot;

    return tempPos;
}

void OdometrySystem::track() {
    rot = sin(Brain.timer(msec) / 1000) * 360;
    xPos = sin(Brain.timer(msec) / 1000) * 10;
    yPos = cos(Brain.timer(msec) / 1000) * 10;

};
