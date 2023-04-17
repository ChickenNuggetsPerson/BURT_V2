#include "robotConfig.h"
#include "odometry.h"
#include "robotMeasurements.h"

using namespace vex;

using std::cout;
using std::endl;


// Position Constructor
Position::Position(double xPos, double yPos, double rotation) {
    x = xPos;
    y = yPos;
    rot = rotation;
};
Position::Position(double xPos, double yPos) {
    x = xPos;
    y = yPos;
    rot = 0;
};
Position::Position() {};

// TilePosition Constructor
TilePosition::TilePosition(double xPos, double yPos, double rotation) {
    x = xPos;
    y = yPos;
    rot = rotation;
};
TilePosition::TilePosition(double xPos, double yPos) {
    x = xPos;
    y = yPos;
    rot = 0;
};
TilePosition::TilePosition() {};





// Main system for tracking the position of the robot

int mainTrackingTask(void* system) {
    OdometrySystem* systemPointer = (OdometrySystem*)system;

    double updateSpeed = 10; // In msec 

    wait(0.5, seconds);

    // V-- Put back in once inertial sensor is installed
    //if (!inertialSensor.installed()) { brainError("Inertial Sensor Not Installed"); return 1;}

    while (inertialSensor.isCalibrating()) {
        wait(0.05, seconds);
    }

    systemPointer->resetEncoders();
    
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

    inertialSensor.startCalibration();

    //trackingTask = vex::task(mainTrackingTask, (void*)this, vex::task::taskPriorityNormal);
}

void OdometrySystem::restart() {
    restart(Position());
};
void OdometrySystem::restart(Position currentPos) {
    if (isTracking) { trackingTask.stop(); isTracking = false; }
    wait(0.1, sec);
    globalX = currentPos.x;
    globalY = currentPos.y;
    globalRot = currentPos.rot;
    lastData = odomRawData();
    
    updateTilePos();
    wait(0.1, sec);
    trackingTask = vex::task(mainTrackingTask, (void*)this, vex::task::taskPriorityNormal);
};
void OdometrySystem::restart(TilePosition currentPos) {
    restart(tilePosToPos(currentPos));
};


Position OdometrySystem::currentPos() {
    return Position(globalX, globalY, globalRot);
};
TilePosition OdometrySystem::currentTilePos() {
    return currentTilePosition;
};


TilePosition OdometrySystem::posToTilePos(Position pos) {
    return TilePosition(
        (pos.x - (tileWidth / 2)) / tileWidth,
        (pos.y - (tileWidth / 2)) / tileWidth,
        pos.rot
    );
};
Position OdometrySystem::tilePosToPos(TilePosition tilePos) {
    return Position(
        (tilePos.x * tileWidth) + (tileWidth / 2), 
        (tilePos.y * tileWidth) + (tileWidth / 2), 
        tilePos.rot);
};

void OdometrySystem::updateTilePos() {
    currentTilePosition = posToTilePos(Position(globalX, globalY, globalRot));
};


void OdometrySystem::resetEncoders() {
    rightMotorA.resetPosition();
    rightMotorB.resetPosition();
    leftMotorA.resetPosition();
    leftMotorB.resetPosition();
    testEncoder.resetRotation();
};

odomRawData OdometrySystem::getChanges(odomRawData oldData) {



    double circumference = 2 * PI * (wheelDiameter / 2);

    // Calc the new encoder positions
    double newRightEncoder = ((rightMotorA.position(rotationUnits::rev) + rightMotorB.position(rotationUnits::rev)) / 2) * circumference;
    double newLeftEncoder = ((leftMotorA.position(rotationUnits::rev) + leftMotorB.position(rotationUnits::rev)) / 2) * circumference;
    double newBackEncoder = (testEncoder.position(rotationUnits::rev)) * circumference;

    odomRawData newData;
    
    // Calculate the arc lengths
    newData.deltaRight = newRightEncoder - oldData.rightEncoder;
    newData.deltaLeft = newLeftEncoder - oldData.leftEncoder;
    newData.deltaBack = newBackEncoder - oldData.backEncoder;
    


 
    // Set the current data
    newData.rightEncoder = newRightEncoder; // Right encoder
    newData.leftEncoder = newLeftEncoder;   // Left encoder
    newData.backEncoder = newBackEncoder;   // Back encoder
    newData.heading = globalRot;  // Heading
    newData.locX = globalX;
    newData.locY = globalY;

    return newData;
}


// Main Tracking Loop
void OdometrySystem::track() {

    // Get the current movement arc
    odomRawData currentData = getChanges(lastData);
    
    // Calc Change in Rotation
    double rotChange = ((currentData.deltaLeft - currentData.deltaRight) / (encoderDist))/2;
    globalRot += rotChange;


    // Code from the Purduesigbots ARMS Library
    // https://github.com/purduesigbots/ARMS/blob/master/src/ARMS/odom.cpp

    double localX;
    double localY;

    if (rotChange != 0.00) {
        double i = sin(rotChange / 2.00) * 2.0;

        localX = (currentData.deltaRight / rotChange - encoderDist) * i;
        localY = (currentData.deltaBack / rotChange + backEncoderDist) * i;

    } else {
        localX = currentData.deltaRight;
        localY = currentData.deltaBack;
    }

    double p = globalRot - rotChange / 2.0;

    globalX += cos(p) * localX - sin(p) * localY;
    globalY += cos(p) * localY + sin(p) * localY;

    //std::cout << globalX << " " << globalY << std::endl;

    lastData = currentData;

    // Make sure to update the tile position
    updateTilePos();
};
