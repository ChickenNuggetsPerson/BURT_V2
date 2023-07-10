
#include "auton.h"
#include "robotConfig.h"
#include "paths.h"
#include <vector>

using namespace vex;


autonPath::autonPath() {};
void autonPath::addMovement(autonMovement movement) {
    movements.push_back(movement);
};

int autonPath::getSize() {
    return movements.size();
}
autonMovement autonPath::getStep(int stepCount) {
    if (stepCount >= movements.size()) {
        return autonMovement(AUTON_MOVE_END);
    } else {
        return movements.at(stepCount);
    }
};



ai::ai(OdometrySystem* systemPointer, aiQueueSystem* queuePtr) {
    odometrySystemPointer = systemPointer;
    queueSystemPtr = queuePtr;
    queuePtr->addPtrs(this, systemPointer);

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
        // Load Configs from SD card
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
        queueSystemPtr->addToQueue(buildPath(AUTON_PATH_TEST, this));
        return;
    }


    // Initalize Auton Variables
    if (getConfig("teamColor")) {
        teamColor = TEAM_RED;
    } else {
        teamColor = TEAM_BLUE;
    }

    generatePath();
};
void ai::generatePath() {
    queueSystemPtr->clear();
    // Build Path
    
    // Old Path System
    /*runningSkills = getConfig("isSkills");
    if (runningSkills) {
        queueSystemPtr->addToQueue(buildPath(AUTON_PATH_SKILLS, this)); 
    } else {
        if (!getConfig("startSide")) {
            queueSystemPtr->addToQueue(buildPath(AUTON_PATH_LEFT, this));
        } else {
            queueSystemPtr->addToQueue(buildPath(AUTON_PATH_RIGHT, this));
        }
        if (!Brain.SDcard.isInserted()) {
            queueSystemPtr->addToQueue(buildPath(AUTON_PATH_TEST, this));
        }
    }*/

    // New Path System
    bool result = false;
    runningSkills = getConfig("isSkills");
    if (runningSkills) {
        result = queueSystemPtr->addToQueue(AUTON_PATH_FOLDER + AUTON_PATH_SKILLS_JSON);
    } else {
        if (!getConfig("startSide")) {
            result = queueSystemPtr->addToQueue(AUTON_PATH_FOLDER + AUTON_PATH_LEFT_JSON);
        } else {
            result = queueSystemPtr->addToQueue(AUTON_PATH_FOLDER + AUTON_PATH_RIGHT_JSON);
        }
        if (!Brain.SDcard.isInserted()) {
            queueSystemPtr->addToQueue(buildPath(AUTON_PATH_TEST, this));
        }
    }
    if (!result) {
        brainError("Error Reading JSON");
    }
}

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






void aiQueueSystem::addPtrs(ai* botAIPtr, OdometrySystem* odometryPointer) {
    aiPtr = botAIPtr;
    odomPtr = odometryPointer;
};
void aiQueueSystem::runQueue() {
    if (running) { return; }
    running = true;

    int i = 0;
    while (i < queue.size()) {
        runMovement(queue.at(i)); 
        i++;
    }

    queue.clear();
    running = false;

}
void aiQueueSystem::clear() {
    queue.clear();
}
std::vector<autonMovement> aiQueueSystem::getQueue() {
    return queue;
};
bool aiQueueSystem::addToQueue(autonPath path) {
    for (int i = 0; i < path.getSize(); i++) {
        queue.push_back(path.getStep(i));
    }
    return true;
}
bool aiQueueSystem::addToQueue(autonMovement movement) {
    queue.push_back(movement);
    return true;
}
bool aiQueueSystem::addToQueue(std::string jsonPath) {
    DynamicJsonDocument* pathPtr = readJsonFromFile(jsonPath.c_str());
    JsonObject path = pathPtr->as<JsonObject>();
    if (path.isNull()) {
        return false;
    }

    JsonArray movementsArray = path["movements"].as<JsonArray>();
    autonPath readPath = autonPath(&botAI);

    // Loop through all objects in the "movements" array
    for (const auto& movement : movementsArray) {
        autonMovement tmpMove;

        // Main stuff
        tmpMove.movementType = movement["type"].as<int>();
        tmpMove.val = movement["val"].as<float>();
        tmpMove.tilePosBool = movement["tilePosBool"].as<bool>();

        // Get Position Data
        JsonObject pos = movement["pos"].as<JsonObject>();
        tmpMove.pos = Position(pos["x"].as<float>(), pos["y"].as<float>(), pos["rot"].as<float>());

        // Get Tile Position Data
        JsonObject tilePos = movement["tilePosition"].as<JsonObject>();
        tmpMove.tilePos = TilePosition(tilePos["x"].as<float>(), tilePos["y"].as<float>(), tilePos["rot"].as<float>());

        // Loop Through Drive Path Array
        JsonArray drivePathArray = movement["drivePath"].as<JsonArray>();


        for (const auto& point : drivePathArray) {
            tmpMove.drivePath.push_back(TilePosition(point["x"].as<float>(), point["y"].as<float>(), point["rot"].as<float>()));
        }

        readPath.addMovement(tmpMove);
    }
    delete pathPtr; // Free Memory
    return addToQueue(readPath);
}
bool aiQueueSystem::runMovement(autonMovement movement) {
    switch (movement.movementType) {
        case AUTON_MOVE_DELAY:
            wait(movement.val, timeUnits::msec);
            return true;
        case AUTON_MOVE_DRIVE_DIST:
            return aiPtr->driveDist(movement.val);
        case AUTON_MOVE_GOTO:
            if (movement.tilePosBool) {
                return aiPtr->gotoLoc(movement.tilePos);
            } else {
                return aiPtr->gotoLoc(movement.pos);
            }
        case AUTON_MOVE_LONGGOTO:
            return aiPtr->longGoto(movement.drivePath);
        case AUTON_MOVE_TURNTO:
            return aiPtr->turnTo(movement.pos.rot);
        case AUTON_MOVE_PICKUP_ACORN:
            return aiPtr->pickupAcorn();
        case AUTON_MOVE_DROPOFF_ACORN:
            return aiPtr->dropAcorn();
        default:
            return false;
    }
}

void aiQueueSystem::autonStarted() {
    brainFancyDebug("Auton Started", vex::color::cyan, true);
    brainChangePage("map");

    // Wait for the odometry system to startup
    while (!odomPtr->isTracking) { wait(0.01, seconds); }

   runQueue();

    // Auton is done?
}