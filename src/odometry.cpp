#include "robotConfig.h"
#include "odometry.h"
#include "math.h"

using namespace vex;
using namespace odom;


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
Position::Position(TilePosition pos) {
    Position tmp = tilePosToPos(pos);
    this->x = tmp.x;
    this->y = tmp.y;
    this->rot = tmp.rot;
};

// TilePosition Constructor
TilePosition::TilePosition(Position pos) {
    TilePosition tmp = posToTilePos(pos);
    this->x = tmp.x;
    this->y = tmp.y;
    this->rot = tmp.rot;
};


// Main system for tracking the position of the robot
int mainTrackingTask(void* system) {
    OdometrySystem* systemPointer = (OdometrySystem*)system;

    // Determine if the encoders are responding
    if (rightEncoder.installed()) {
        if (leftEncoder.installed()) {
            systemPointer->usingDrive = false;
            systemPointer->usingOneOdom = false;
        } else {
            systemPointer->usingDrive = false;
            systemPointer->usingOneOdom = true;
        }
    } else {
        systemPointer->usingDrive = true;
        systemPointer->usingOneOdom = false;
    }


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
    // inertialSensor.setRotation(180, vex::rotationUnits::deg);
    // brainFancyDebug("Calibrated", color::green, false);

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
        wait(systemPointer->timeBetweenTrack, sec);
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
    restart(Position(currentPos));
};

void OdometrySystem::setPos(Position newPos) {
    globalX = newPos.x;
    globalY = newPos.y;
    globalRot = newPos.rot;
};
void OdometrySystem::setPos(TilePosition newPos) {
    setPos(Position(newPos));
};


// Return current position
Position OdometrySystem::currentPos() {
    return Position(globalX, globalY, globalRot);
};
TilePosition OdometrySystem::currentTilePos() {
    return currentTilePosition;
};
double OdometrySystem::getVelocity() {
    return velocity;
};


void OdometrySystem::updateTilePos() {
    currentTilePosition = TilePosition(Position(globalX, globalY, globalRot));
};

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
        newRightEncoder = (rightMotorB.position(rotationUnits::rev) / wheelGearTeeth ) * motorGearTeeth * circumference;
        newLeftEncoder = (leftMotorB.position(rotationUnits::rev) / wheelGearTeeth ) * motorGearTeeth * circumference;
    } else {
        if (usingOneOdom) {
            newRightEncoder = rightEncoder.position(rotationUnits::rev) * circumference;
            newLeftEncoder = newRightEncoder;
        } else {
            newRightEncoder = rightEncoder.position(rotationUnits::rev) * circumference;
            newLeftEncoder = leftEncoder.position(rotationUnits::rev) * circumference;
        }
    }
    // Use all four drivetrain wheels
    //double newRightEncoder = ((rightMotorA.position(rotationUnits::rev) + rightMotorB.position(rotationUnits::rev)) / 2) * circumference;
    //double newLeftEncoder = ((leftMotorA.position(rotationUnits::rev) + leftMotorB.position(rotationUnits::rev)) / 2) * circumference;
    
    odomRawData newData;
    
    // Calculate the arc lengths
    newData.deltaRight = newRightEncoder - oldData.rightEncoder;
    newData.deltaLeft = newLeftEncoder - oldData.leftEncoder;

    newData.heading = inertialAverager.iterate(
        misc::degreeToRad(inertialSensor.rotation(rotationUnits::deg))
    );    
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
    globalRot = currentData.heading;

    // Change in distance
    double deltaDist = (currentData.deltaLeft + currentData.deltaRight) / 2.0;
    velocity = deltaDist / timeBetweenTrack;

    // Use Trig to find change in X and Y
    // Apply deltas to the global positions
    globalX += deltaDist * sin(currentData.heading);
    globalY += deltaDist * cos(currentData.heading);    
    
    lastData = currentData;

    // Make sure to update the tile position
    updateTilePos();
};
