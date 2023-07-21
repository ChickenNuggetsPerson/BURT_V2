#include "robotConfig.h"
#include "odometry.h"
#include "math.h"

using namespace vex;
using namespace odom;

using std::cout;
using std::endl;


TilePosition odom::posToTilePos(Position pos) {
    return TilePosition(
        (pos.x - (tileWidth / 2)) / tileWidth,
        (pos.y - (tileWidth / 2)) / tileWidth,
        pos.rot);
};
Position odom::tilePosToPos(TilePosition tilePos) {
    return Position(
        (tilePos.x * tileWidth) + (tileWidth / 2), 
        (tilePos.y * tileWidth) + (tileWidth / 2), 
        tilePos.rot);
};


// Position Constructor
Position::Position(double xPos, double yPos, double rotation) {
    x = xPos;
    y = yPos;
    rot = rotation;
};
Position::Position(double xPos, double yPos) {
    x = xPos;
    y = yPos;
    rot = NAN;
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
    rot = NAN;
};
TilePosition::TilePosition() {};





// Main system for tracking the position of the robot

int mainTrackingTask(void* system) {
    OdometrySystem* systemPointer = (OdometrySystem*)system;

    // Determine if the encoders are responding
    systemPointer->usingDrive = !(leftEncoder.installed() && rightEncoder.installed());

    double updateSpeed = 10; // In msec 

    brainFancyDebug("Starting Calibration", color::yellow, true);
    if (!systemPointer->firstTime) {
        inertialSensor.startCalibration();
        systemPointer->firstTime = false;
    } else {
        inertialSensor.startCalibration();
    }
    while (inertialSensor.isCalibrating()) { // Wait for inertial sensor to calibrate
        wait(0.02, seconds);
    }
    brainFancyDebug("Calibrated", color::green);

    systemPointer->resetEncoders();
    
    vex::color notColor;
    notColor.rgb(24, 222, 166);
    brainFancyDebug("Starting Odometry System", notColor, true);
    
    systemPointer->isTracking = true;
    if (systemPointer->usingDrive) {
        brainError("Using Drive for Odom");
    }

    while (true) {
        systemPointer->track(); // Find main tracking calculations here 
        wait(updateSpeed, msec);
    }
    
    return 1;
}

OdometrySystem::OdometrySystem() {
       
}

void OdometrySystem::restart() {
    restart(ODOM_DEFAULT_RESET_POS);
};
void OdometrySystem::restart(Position currentPos) {
    if (isTracking) { trackingTask.stop(); isTracking = false; } // Stop tracking task
    
    // Set global pos to the new position
    globalX = currentPos.x;
    globalY = currentPos.y;
    globalRot = currentPos.rot;
    lastData = odomRawData();

    updateTilePos();
    
    // Start tracking task
    trackingTask = vex::task(mainTrackingTask, (void*)this, vex::task::taskPriorityNormal);
};
void OdometrySystem::restart(TilePosition currentPos) {
    restart(tilePosToPos(currentPos));
};

void OdometrySystem::setPos(Position newPos) {
    globalX = newPos.x;
    globalY = newPos.y;
    globalRot = newPos.rot;
};
void OdometrySystem::setPos(TilePosition newPos) {
    setPos(tilePosToPos(newPos));
};


// Return current position
Position OdometrySystem::currentPos() {
    return Position(globalX, globalY, globalRot);
};
TilePosition OdometrySystem::currentTilePos() {
    return currentTilePosition;
};



void OdometrySystem::updateTilePos() {
    currentTilePosition = posToTilePos(Position(globalX, globalY, globalRot));
};


// Keeps a running average of the last 10 readings of the inertial sensor
void OdometrySystem::calcInertialAvg() {
    for (int i=0; i < inertialAvgSize - 1; i++) {
        inertialLastVals[i] = inertialLastVals[i + 1];
    }
    inertialLastVals[inertialAvgSize - 1] = misc::degreeToRad(inertialSensor.rotation(rotationUnits::deg));
    double tempAvg = 0.00;
    for (int i=0; i < inertialAvgSize; i++) {
        tempAvg += inertialLastVals[i];
    }
    inertialAvg = tempAvg / inertialAvgSize;
}


// Reset the encoders
void OdometrySystem::resetEncoders() {
    rightMotorA.resetPosition();
    rightMotorB.resetPosition();
    leftMotorA.resetPosition();
    leftMotorB.resetPosition();
    
    leftEncoder.resetPosition();
    rightEncoder.resetPosition();
};


// Calculate the changes from the last position
odomRawData OdometrySystem::getChanges(odomRawData oldData) {
    
    double circumference = 2 * PI * (wheelDiameter / 2);

    // Calc the new encoder positions
    double newRightEncoder;
    double newLeftEncoder; 

    if (usingDrive) {
        newRightEncoder = (rightMotorB.position(rotationUnits::rev) / wheelRatio ) * motorRatio * circumference;
        newLeftEncoder = (leftMotorB.position(rotationUnits::rev) / wheelRatio ) * motorRatio * circumference;
    } else {
        newRightEncoder = rightEncoder.position(rotationUnits::rev) * circumference;
        newLeftEncoder = leftEncoder.position(rotationUnits::rev) * circumference;
    }
    // Use all four drivetrain wheels
    //double newRightEncoder = ((rightMotorA.position(rotationUnits::rev) + rightMotorB.position(rotationUnits::rev)) / 2) * circumference;
    //double newLeftEncoder = ((leftMotorA.position(rotationUnits::rev) + leftMotorB.position(rotationUnits::rev)) / 2) * circumference;
    
    odomRawData newData;
    
    // Calculate the arc lengths
    newData.deltaRight = newRightEncoder - oldData.rightEncoder;
    newData.deltaLeft = newLeftEncoder - oldData.leftEncoder;
    calcInertialAvg();
    newData.heading = inertialAvg;    
    newData.deltaHeading = newData.heading - oldData.heading;

 
    // Set the current data
    newData.rightEncoder = newRightEncoder; // Right encoder
    newData.leftEncoder = newLeftEncoder;   // Left encoder
    newData.locX = globalX;
    newData.locY = globalY;

    return newData;
}


// Main Tracking Calculations
void OdometrySystem::track() {

    // Get the current movement arc
    odomRawData currentData = getChanges(lastData);
    
    // Calc Change in Rotation
    //double rotChange = ((currentData.deltaLeft - currentData.deltaRight) / (encoderDist))/2;
    globalRot = currentData.heading;

    // Change in distance
    double deltaDist = (currentData.deltaLeft + currentData.deltaRight) / 2.0;

    // Use Trig to find change in X and Y
    double deltaX = deltaDist * sin(currentData.heading);
    double deltaY = deltaDist * cos(currentData.heading);

    // Apply deltas to the global positions
    globalX += deltaX;
    globalY += deltaY;    

    lastData = currentData;

    // Make sure to update the tile position
    updateTilePos();
};
