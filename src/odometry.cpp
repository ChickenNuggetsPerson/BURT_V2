#include "robotConfig.h"
#include "odometry.h"

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
    currentPosition = currentPos;
    updateTilePos();
    wait(0.1, sec);
    trackingTask = vex::task(mainTrackingTask, (void*)this, vex::task::taskPriorityNormal);
};
void OdometrySystem::restart(TilePosition currentPos) {
    restart(tilePosToPos(currentPos));
};


Position OdometrySystem::currentPos() {
    return currentPosition;
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
    currentTilePosition = posToTilePos(currentPosition);
};




void OdometrySystem::track() {
    currentPosition.rot = inertialSensor.rotation(rotationUnits::deg);


    // Todo: research methods for tracking movement
    //       maybe use the encoders in the motors until we get rotational sensors


    // Two different types of movements to track, linear and arc movements
    // Linear = Easy because trig 
    // Arc = Headache because I do not know that type of math yet

    currentPosition.x = 70 + sin(Brain.timer(msec) / 1000) * 70;
    currentPosition.y = 70 + cos(Brain.timer(msec) / 1000) * 70;


    // Make sure to update the tile position
    updateTilePos();
};
