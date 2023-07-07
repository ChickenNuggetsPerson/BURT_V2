
#include "auton.h"
#include "robotConfig.h"
#include "paths.h"
#include <vector>

using namespace vex;


autonPath::autonPath() {};
autonPath::autonPath(ai* autonPointer) {
    pointer = autonPointer;
};
void autonPath::addMovement(autonMovement movement) {
    movements[totalMovements] = movement;
    totalMovements++;
};

bool autonPath::runMovement(int movementNum) {
    autonMovement movement = movements[movementNum];

    switch (movement.movementType) {
        case AUTON_DELAY:
            wait(movement.val, timeUnits::msec);
            return true;
        case AUTON_DRIVE_DIST:
            return pointer->driveDist(movement.val);
        case AUTON_GOTO:
            if (movement.tilePosBool) {
                return pointer->gotoLoc(movement.tilePos);
            } else {
                return pointer->gotoLoc(movement.pos);
            }
        case AUTON_LONGGOTO:
            return pointer->longGoto(movement.drivePath);
        case AUTON_TURNTO:
            return pointer->turnTo(movement.pos.rot);
        case AUTON_PICKUP_ACORN:
            return pointer->pickupAcorn();
        case AUTON_DROPOFF_ACORN:
            return pointer->dropAcorn();
        default:
            return false;
    }

};

void autonPath::reset() {
    currentStep = 0;
}
bool autonPath::step() {
    if (currentStep >= totalMovements) {
        return false;
    }
    currentStep++;
    return runMovement(currentStep - 1);
};
autonMovement autonPath::getStep(int stepCount) {
    if (stepCount >= totalMovements) {
        return autonMovement(AUTON_END);
    } else {
        return movements[stepCount];
    }
};



ai::ai(OdometrySystem* systemPointer) {
    odometrySystemPointer = systemPointer;

    // Define the Config
    configStorage.push_back(autonConfig("teamColor", "Red", "Blue", false, vex::color(247, 30, 54), vex::color(62, 133, 247)));
    configStorage.push_back(autonConfig("startSide", "Left", "Right", false, vex::color(50, 50, 50), vex::color(0, 0, 0)));
    configStorage.push_back(autonConfig("isSkills", "Match", "Skills", false));
    
}

// Load Auton Configs
void ai::init() {

    if (loaded) { 
        // Reset auton config
        loaded = false;
        brainFancyDebug("Auton Reset", blue, true);
    }

    // Load auton Config
    if (Brain.SDcard.isInserted()) {
        for (auto &config: configStorage) {
            unsigned int valueRead = readFile((configFoler + config.id + configFileType).c_str());
            config.status = ( valueRead == 1);
        }
        wait(0.1, seconds);
        loaded = true;
        brainFancyDebug("Auton Initialized", cyan, true);
        odometrySystemPointer->restart(getStartPos());
    } else {
        brainError("No SD Card");
        odometrySystemPointer->restart();
        path = buildPath(AUTON_PATH_TEST, this);
        return;
    }


    // Initalize Auton Variables
    if (getConfig("teamColor")) {
        teamColor = TEAM_RED;
    } else {
        teamColor = TEAM_BLUE;
    }


    // Build Path
    runningSkills = getConfig("isSkills");
    if (runningSkills) {
        path = buildPath(AUTON_PATH_SKILLS, this);
    } else {
        if (!getConfig("startSide")) {
            path = buildPath(AUTON_PATH_LEFT, this);
        } else {
            path = buildPath(AUTON_PATH_RIGHT, this);
        }
        if (!Brain.SDcard.isInserted()) {
            path = buildPath(AUTON_PATH_TEST, this);
        }
    }
    
    
};


// Return the config value based on the name
bool ai::getConfig(const char* configId) {
    for (auto config: configStorage) {
        if (config.id == std::string(configId)) {
            return config.status;
        }
    }
    return false;
};

// Saves the value to the sd card
// Does not update internal storage so make sure to reintialize auton after writing configs
void ai::saveConfig(const char* configId, bool value) {

        std::cout << "Saving: " << configId << " " << value << std::endl;

        for (auto config: configStorage) {
        if (config.id == std::string(configId)) {
            int writeVal = 0;
            if (value) { writeVal = 1;}
            writeFile((configFoler + config.id + configFileType).c_str(), writeVal);
        }
    }

};



// Returns the auton start position based on the config
Position ai::getStartPos() {

    // Idea: if the robot starts against the wall, use the distance sensor to measure distance
    //           from perpendicular wall, which will provide a more accurate starting position

    if (!Brain.SDcard.isInserted()) { return Position(); }

    if (runningSkills) {
        return odometrySystemPointer->tilePosToPos(AUTON_START_SKILLS);
    }
    if (!getConfig("startSide")) {
        return odometrySystemPointer->tilePosToPos(AUTON_START_LEFT);
    } else {
        return odometrySystemPointer->tilePosToPos(AUTON_START_RIGHT);
    }
}

// Returns true if the autonomous is ready
bool ai::isReady() {return loaded;};

// Returns if the auton is configures for skills
bool ai::isRunningSkills() {return runningSkills;}

// Finds the nearest rotation based on the provided target absolute rotation
// USES DEGREES AND NOT RADIANS
double ai::findNearestRot(double currentRot, double targetRot) {

    int currentMult = trunc(currentRot / 360);

    double smallerVal = (( currentMult - 1 ) * 360) + targetRot;
    double middleVal = (( currentMult ) * 360) + targetRot;
    double biggerVal = (( currentMult + 1 ) * 360) + targetRot;

    double smallerDiff = fabs(smallerVal - currentRot);
    double middleDiff = fabs(middleVal - currentRot);
    double biggerDiff = fabs(biggerVal - currentRot);

    if (smallerDiff < middleDiff && smallerDiff < biggerDiff) {
        return smallerVal;
    }
    if (middleDiff < smallerDiff && middleDiff < biggerDiff) {
        return middleVal;
    }
    if (biggerDiff < smallerDiff && biggerDiff < middleDiff) {
        return biggerVal;
    }
    return middleVal;
};

// https://www.desmos.com/calculator/gdryojf8i3 << My tests for figuring out the math
// Returns Angle between points in radians using positive Y axis as 0 and goes clockwise
double ai::angleBetweenPoints(Position pos1, Position pos2) {
    //std::cout << atan2(pos2.x - pos1.x, pos2.y - pos1.y) << std::endl;
    return atan2(pos2.x - pos1.x, pos2.y - pos1.y);
}
// Returns the distance between points
double ai::distBetweenPoints(Position pos1, Position pos2) {
    return sqrt(pow((pos2.x - pos1.x), 2) + pow((pos2.y - pos1.y), 2));
}

// Returns the position the auton is driving to
Position ai::getTargetPos() {
    return target;
}



// Drives the distance
// Math Visualization: https://www.desmos.com/calculator/uywraxwtws
bool ai::driveDist(double dist) {
    Position currentPosition = odometrySystemPointer->currentPos();
    double desiredX = currentPosition.x + dist * cos(2.5*PI - currentPosition.rot);
    double desiredY = currentPosition.y + dist * sin(2.5*PI - currentPosition.rot);
    return gotoLoc(Position(desiredX, desiredY, NAN));
}
// Turns to the desired rotation in degrees
bool ai::turnTo(double deg) {
    return turnTo(deg, 2);
}
// Turns to the desired rotation in degrees
bool ai::turnTo(double deg, double turnTimeout) {
    
    if (!odometrySystemPointer->isTracking) { 
        brainError("Skipping Auton Path, Odom not initialized");
        return false; 
    }

    double timeout = Brain.timer(msec) + (turnTimeout * 1000);

    bool wasRunning = running;
    running = true;

    double heading = radToDegree(odometrySystemPointer->currentPos().rot);
    double target = findNearestRot(heading, deg);
    PID turnPID(AUTON_TURNTO_PID_CONFIG, target);


    double accuracy = 0.5;
    int checks = 3;


    double lastRot = odometrySystemPointer->currentPos().rot;
    int totalChecks = 0;
    while (true) {
        
        heading = radToDegree(odometrySystemPointer->currentPos().rot);
        double power = turnPID.iterate(heading);

        LeftDriveSmart.spin(directionType::fwd, -power, volt);
        RightDriveSmart.spin(directionType::fwd, power, volt);

        double deltaHeading = lastRot - heading;
        if (deltaHeading <= accuracy && deltaHeading >= -accuracy) {
            totalChecks++;
            if (totalChecks > checks) {
                break;
            }
        }

        if (Brain.timer(msec) > timeout) {
            break;
        }

        lastRot = heading;
        
        wait(0.05, seconds);
    }

    LeftDriveSmart.spin(directionType::fwd, 0, volt);
    RightDriveSmart.spin(directionType::fwd, 0, volt);

    running = wasRunning;
    return true;
};



bool ai::gotoLoc(TilePosition pos) {
    return gotoLoc(odometrySystemPointer->tilePosToPos(pos));
};
bool ai::gotoLoc(Position pos) {
    std::vector<Position> tmp;
    tmp.push_back(pos);
    return longGoto(tmp);
};
bool ai::longGoto(std::vector<TilePosition> pos) {
    std::vector<Position> tmpVec;
    for (int i = 0; i < pos.size(); i++) {
        tmpVec.push_back(odometrySystemPointer->tilePosToPos(pos.at(i)));
    }
    return longGoto(tmpVec);
};

// Main goto logic
bool ai::longGoto(std::vector<Position> pos) {

    if (!odometrySystemPointer->isTracking) { 
        brainError("Skipping Auton Path, Odom not initialized");
        return false; 
    }

    bool wasRunning = running;
    running = true;
    
    target.x = pos.at(0).x;
    target.y = pos.at(0).y;
    target.rot = pos.at(0).rot;

    Position currentPos = odometrySystemPointer->currentPos();

    double travelDist = distBetweenPoints(currentPos, pos.at(0));
    double desiredHeading = radToDegree(angleBetweenPoints(currentPos, pos.at(0)));

    turnTo(desiredHeading, 1.5);

    // PID to control drive speed
    PID drivePid(AUTON_GOTO_DRIVE_PID_CONFIG, 0);
    drivePid.setMax(12);
    drivePid.setMin(-12);
    double drivePower = 0.00;

    // PID to keep the robot driving straight
    PID turnPid(AUTON_GOTO_TURN_PID_CONFIG);
    double turnPower = 0.00;

    int stopped = 0;
    bool traveling = true;

    int targetNum = 0;
    int numOfTargets = pos.size();

    int speedUp = 10;

    while (traveling) {
        
        Position tempPos = odometrySystemPointer->currentPos();

        travelDist = distBetweenPoints(tempPos, pos.at(targetNum));

        if (targetNum != numOfTargets - 1) {
            travelDist = travelDist + 10;
        }

        desiredHeading = radToDegree(angleBetweenPoints(tempPos, pos.at(targetNum)));

        drivePower = drivePid.iterate(travelDist / speedUp);

        double turnCurrent = radToDegree(odometrySystemPointer->currentPos().rot);
        double turnWant = findNearestRot(radToDegree(tempPos.rot), desiredHeading);
        turnPower = turnPid.iterate(turnCurrent, turnWant);

        double leftPower = drivePower - turnPower;
        double rightPower = drivePower + turnPower;

        LeftDriveSmart.spin(directionType::fwd, leftPower, voltageUnits::volt);
        RightDriveSmart.spin(directionType::fwd, rightPower, voltageUnits::volt);

        if (drivePower < 2 && speedUp < 2) {
            stopped++;
        }

        if (targetNum != numOfTargets - 1 && travelDist < 15) {
            stopped = 0;
            targetNum++;
            if (targetNum == numOfTargets) {            
                target = pos.at(targetNum - 1);
            } else {
                target = pos.at(targetNum);
            }
        }
        if (stopped > 10) {
            stopped = 0;
            targetNum++;
            if (targetNum == numOfTargets) {            
                target = pos.at(targetNum - 1);
            } else {
                target = pos.at(targetNum);
            }
        }

        if (targetNum >= numOfTargets) {
            traveling = false;
        }

        speedUp--;
        if (speedUp < 1) {speedUp = 1;}

        wait(0.05, seconds);
    }

    if (!isnan(pos.at(pos.size() - 1).rot)) {
        turnTo(pos.at(pos.size() - 1).rot, 1.5);
    }
    
    running = wasRunning;
    return true;
};

bool ai::pickupAcorn() {

    if (!odometrySystemPointer->isTracking) {
        brainError("Skipping Auton Path, Odom not initialized");
        return false;
    }
    bool wasrunning = running;
    running = true;

    // Pickup Acorn Logic
    
    // Implement this once the robot is built


    running = wasrunning;
    return true;

};
bool ai::dropAcorn() {

    if (!odometrySystemPointer->isTracking) {
        brainError("Skipping Auton Path, Odom not initialized");
        return false;
    }
    bool wasrunning = running;
    running = true;

    // Drop Acorn Logic
    
    // Implement this once the robot is built


    running = wasrunning;
    return true;
}

void ai::stop() {
    running = false;
}

bool ai::playPath(autonPath path) {
    return true;
}


// Run when the autonomous period is started
void ai::started() {
    brainFancyDebug("Auton Started", vex::color::cyan, true);
    brainChangePage("map");

    while (!odometrySystemPointer->isTracking) { wait(0.01, seconds); }

    path.reset();

    // Step throught the path
    while (path.step()) {}

    // Auton is done?
};


