#include "robotConfig.h"
#include "odometry.h"
#include "math.h"

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

    if (!systemPointer->firstTime) {
        inertialSensor.startCalibration();
        systemPointer->firstTime = false;
    } else {
        inertialSensor.startCalibration();
    }
    while (inertialSensor.isCalibrating()) {
        wait(0.02, seconds);
    }

    systemPointer->resetEncoders();
    
    vex::color notColor;
    notColor.rgb(24, 222, 166);
    brainFancyDebug("Starting Odometry System", notColor, true);
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

void OdometrySystem::restart() {
    restart(ODOM_DEFAULT_RESET_POS);
};
void OdometrySystem::restart(Position currentPos) {
    if (isTracking) { trackingTask.stop(); isTracking = false; }
    globalX = currentPos.x;
    globalY = currentPos.y;
    globalRot = currentPos.rot;
    lastData = odomRawData();
    updateTilePos();
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



void OdometrySystem::calcInertialAvg() {
    for (int i=0; i < inertialAvgSize - 1; i++) {
        inertialLastVals[i] = inertialLastVals[i + 1];
    }
    inertialLastVals[inertialAvgSize - 1] = degreeToRad(inertialSensor.rotation(rotationUnits::deg));
    double tempAvg = 0.00;
    for (int i=0; i < inertialAvgSize; i++) {
        tempAvg += inertialLastVals[i];
    }
    inertialAvg = tempAvg / inertialAvgSize;
}


void OdometrySystem::resetEncoders() {
    rightMotorA.resetPosition();
    rightMotorB.resetPosition();
    leftMotorA.resetPosition();
    leftMotorB.resetPosition();
};

odomRawData OdometrySystem::getChanges(odomRawData oldData) {
    
    double circumference = 2 * PI * (wheelDiameter / 2);

    // Calc the new encoder positions
    //double newRightEncoder = ((rightMotorA.position(rotationUnits::rev) + rightMotorB.position(rotationUnits::rev)) / 2) * circumference;
    //double newLeftEncoder = ((leftMotorA.position(rotationUnits::rev) + leftMotorB.position(rotationUnits::rev)) / 2) * circumference;
    
    double newRightEncoder = rightMotorB.position(rotationUnits::rev) * circumference;
    double newLeftEncoder = leftMotorB.position(rotationUnits::rev) * circumference;
    
    //double newBackEncoder = (testEncoder.position(rotationUnits::rev)) * circumference;

    odomRawData newData;
    
    // Calculate the arc lengths
    newData.deltaRight = newRightEncoder - oldData.rightEncoder;
    newData.deltaLeft = newLeftEncoder - oldData.leftEncoder;
    //newData.deltaBack = newBackEncoder - oldData.backEncoder;
    calcInertialAvg();
    newData.heading = inertialAvg;    
    newData.deltaHeading = newData.heading - oldData.heading;

 
    // Set the current data
    newData.rightEncoder = newRightEncoder; // Right encoder
    newData.leftEncoder = newLeftEncoder;   // Left encoder
    //newData.backEncoder = newBackEncoder;   // Back encoder
    //newData.heading = globalRot;  // Heading
    newData.locX = globalX;
    newData.locY = globalY;

    return newData;
}


// Main Tracking Loop
void OdometrySystem::track() {

    // Get the current movement arc
    odomRawData currentData = getChanges(lastData);
    
    // Calc Change in Rotation
    //double rotChange = ((currentData.deltaLeft - currentData.deltaRight) / (encoderDist))/2;
    globalRot = currentData.heading;


    double deltaDist = (currentData.deltaLeft + currentData.deltaRight) / 2.0;

    double deltaX = deltaDist * sin(currentData.heading);
    double deltaY = deltaDist * cos(currentData.heading);

    globalX += deltaX;
    globalY += deltaY;    

    //std::cout << std::endl;
    //std::cout << deltaX << " " << deltaY << " " << currentData.heading << std::endl;
    //std::cout << globalX << " " << globalY << " " << globalRot << std::endl;

    lastData = currentData;

    // Make sure to update the tile position
    updateTilePos();
};
