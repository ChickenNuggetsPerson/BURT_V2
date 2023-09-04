
#include "auton.h"
#include "robotConfig.h"
#include "paths.h"
#include <vector>

#include "visionSensorConfig.h"

using namespace vex;
using namespace auton;

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



AutonSystem::AutonSystem(odom::OdometrySystem* systemPointer, aiQueueSystem* queuePtr) {
    odometrySystemPointer = systemPointer;
    queueSystemPtr = queuePtr;
    queuePtr->addPtrs(this, systemPointer);

    // Define the Config
    configStorage.push_back(autonConfig("teamColor", "Red", "Blue", false, vex::color(247, 30, 54), vex::color(62, 133, 247)));
    configStorage.push_back(autonConfig("startSide", "Left", "Right", false, vex::color(50, 50, 50), vex::color(0, 0, 0)));
    configStorage.push_back(autonConfig("isSkills", "Match", "Skills", false));
    
}

int genTask() { // For some reason if it is generated twice, it fixes all issues
    botAI.generatePath();
    return 1;
}

// Load Auton Configs
void AutonSystem::init() {

    if (loaded) { 
        // Reset auton config
        loaded = false;
        brainFancyDebug("Auton Reset", blue, true);
    }

    // Load auton Config
    if (Brain.SDcard.isInserted()) {
        // Load Configs from SD card
        for (auto &config: configStorage) {
            unsigned int valueRead = misc::readFile((configFoler + config.id + configFileType).c_str());
            config.status = ( valueRead == 1);
        }
        wait(0.1, seconds);
        loaded = true;
        brainFancyDebug("Auton Initialized", cyan, true);
    } else {
        brainError("No SD Card");
        odometrySystemPointer->restart();
        queueSystemPtr->addToQueue(buildPath(AUTON_PATH_TEST));
        return;
    }

    odometrySystemPointer->restart();

    // Initalize Auton Variables
    if (getConfig("teamColor")) {
        teamColor = TEAM_RED;
    } else {
        teamColor = TEAM_BLUE;
    }

    vex::task generateTask(genTask);
    //generatePath();
};
void AutonSystem::generatePath() {
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
            queueSystemPtr->addToQueue(buildPath(AUTON_PATH_TEST));
        }
    }
    if (!result) {
        brainError("Error Reading JSON");
    }
    odometrySystemPointer->setPos(getStartPos());
}

// Return the config value based on the name
bool AutonSystem::getConfig(const char* configId) {
    for (auto config: configStorage) {
        if (config.id == std::string(configId)) {
            return config.status;
        }
    }
    return false;
};

// Saves the value to the sd card
// Does not update internal storage so make sure to reintialize auton after writing configs
void AutonSystem::saveConfig(const char* configId, bool value) {

        //std::cout << "Saving: " << configId << " " << value << std::endl;
        DEBUGLOG("SAVING: ", configId, " ", value);

        for (auto config: configStorage) {
        if (config.id == std::string(configId)) {
            int writeVal = 0;
            if (value) { writeVal = 1;}
            misc::writeFile((configFoler + config.id + configFileType).c_str(), writeVal);
        }
    }

};

// Returns the auton start position based on the config
odom::Position AutonSystem::getStartPos() {

    // Idea: if the robot starts against the wall, use the distance sensor to measure distance
    //           from perpendicular wall, which will provide a more accurate starting position

    return odom::tilePosToPos(startPos);
}

void AutonSystem::setStartPos(odom::TilePosition pos) {
    startPos = pos;
};
void AutonSystem::setStartPos() {
    if (!Brain.SDcard.isInserted()) { startPos = odom::TilePosition(); return;}

    if (runningSkills) {
        startPos = AUTON_START_SKILLS;
    }
    if (!getConfig("startSide")) {
        startPos = AUTON_START_LEFT;
    } else {
        startPos = AUTON_START_RIGHT;
    }
};


// Returns true if the autonomous is ready
bool AutonSystem::isReady() {return loaded;};

// Returns if the auton is configures for skills
bool AutonSystem::isRunningSkills() {return runningSkills;}

// Finds the nearest rotation based on the provided target absolute rotation
// USES DEGREES AND NOT RADIANS
double AutonSystem::findNearestRot(double currentRot, double targetRot) {

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
inline double AutonSystem::angleBetweenPoints(odom::Position pos1, odom::Position pos2) {
    //std::cout << atan2(pos2.x - pos1.x, pos2.y - pos1.y) << std::endl;
    return atan2(pos2.x - pos1.x, pos2.y - pos1.y);
}
// Returns the distance between points
inline double AutonSystem::distBetweenPoints(odom::Position pos1, odom::Position pos2) {
    return sqrt(pow((pos2.x - pos1.x), 2) + pow((pos2.y - pos1.y), 2));
}

// Returns the position the auton is driving to
odom::Position AutonSystem::getTargetPos() {
    return target;
}



// Drives the distance
// Math Visualization: https://www.desmos.com/calculator/uywraxwtws
bool AutonSystem::driveDist(double dist) {
    odom::Position currentPosition = odometrySystemPointer->currentPos();
    double desiredX = currentPosition.x + dist * cos(2.5*PI - currentPosition.rot);
    double desiredY = currentPosition.y + dist * sin(2.5*PI - currentPosition.rot);
    return gotoLoc(odom::Position(desiredX, desiredY, NAN));
}
// Turns to the desired rotation in degrees
bool AutonSystem::turnTo(double deg) {
    return turnTo(deg, 2);
}
// Turns to the desired rotation in degrees
bool AutonSystem::turnTo(double deg, double turnTimeout) {
    
    if (!odometrySystemPointer->isTracking) { 
        brainError("Skipping Auton Path, Odom not initialized");
        return false; 
    }

    double timeout = Brain.timer(msec) + (turnTimeout * 1000);

    bool wasRunning = running;
    running = true;

    double heading = misc::radToDegree(odometrySystemPointer->currentPos().rot);
    double target = findNearestRot(heading, deg);
    pid::PID turnPID(AUTON_TURNTO_PID_CONFIG, target);


    double accuracy = 0.5;
    int checks = 3;


    double lastRot = odometrySystemPointer->currentPos().rot;
    int totalChecks = 0;
    while (true) {
        
        heading = misc::radToDegree(odometrySystemPointer->currentPos().rot);
        double power = turnPID.iterate(heading);

        if (WSDebugger.isSending()) {
            WSDebugger.sendData("T", power);
        }

        LeftDriveSmart.spin(directionType::fwd, -power, volt);
        RightDriveSmart.spin(directionType::fwd, power, volt);

        DEBUGLOG("TURNTO PID: ", power);

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



bool AutonSystem::gotoLoc(odom::TilePosition pos) {
    return gotoLoc(odom::tilePosToPos(pos));
};
bool AutonSystem::gotoLoc(odom::Position pos) {
    std::vector<odom::Position> tmp;
    tmp.push_back(pos);
    return longGoto(tmp);
};
bool AutonSystem::longGoto(std::vector<odom::TilePosition> pos) {
    std::vector<odom::Position> tmpVec;
    for (int i = 0; i < pos.size(); i++) {
        tmpVec.push_back(odom::tilePosToPos(pos.at(i)));
    }
    return longGoto(tmpVec);
};

// Main goto logic
bool AutonSystem::longGoto(std::vector<odom::Position> pos) {

    if (!odometrySystemPointer->isTracking) { 
        brainError("Skipping Auton Path, Odom not initialized");
        return false; 
    }

    bool wasRunning = running;
    running = true;
    
    target.x = pos.at(0).x;
    target.y = pos.at(0).y;
    target.rot = pos.at(0).rot;

    odom::Position currentPos = odometrySystemPointer->currentPos();

    double travelDist = distBetweenPoints(currentPos, pos.at(0));
    double desiredHeading = misc::radToDegree(angleBetweenPoints(currentPos, pos.at(0)));

    // Turn to point to the location
    turnTo(desiredHeading, 1.5);

    // PID to control drive speed
    pid::PID drivePid(AUTON_GOTO_DRIVE_PID_CONFIG, 0);
    drivePid.setMax(12);
    drivePid.setMin(-12);
    double drivePower = 0.00;

    // PID to keep the robot driving straight
    pid::PID turnPid(AUTON_GOTO_TURN_PID_CONFIG);
    turnPid.setMax(12);
    turnPid.setMin(-12);
    double turnPower = 0.00;

    int stopped = 0;
    bool traveling = true;

    int targetNum = 0;
    int numOfTargets = pos.size();

    int speedUp = 10;

    // Main Driving Loop
    while (traveling) {
        
        odom::Position tempPos = odometrySystemPointer->currentPos();

        travelDist = distBetweenPoints(tempPos, pos.at(targetNum));

        if (targetNum != numOfTargets - 1) { // Figure this out
            //travelDist = travelDist + 3;
        }

        desiredHeading = misc::radToDegree(angleBetweenPoints(tempPos, pos.at(targetNum)));

        drivePower = drivePid.iterate(travelDist / speedUp);

        double turnCurrent = misc::radToDegree(odometrySystemPointer->currentPos().rot);
        double turnWant = findNearestRot(misc::radToDegree(tempPos.rot), desiredHeading);
        turnPower = turnPid.iterate(turnCurrent, turnWant);

        double leftPower = drivePower - turnPower;
        double rightPower = drivePower + turnPower;

        LeftDriveSmart.spin(directionType::fwd, leftPower, voltageUnits::volt);
        RightDriveSmart.spin(directionType::fwd, rightPower, voltageUnits::volt);

        if (WSDebugger.isSending()) {
            WSDebugger.sendData("DP", drivePower);
            WSDebugger.sendData("TP", turnPower);
        }
        //std::cout << travelDist << " " << drivePower << " " << turnPower << std::endl;
        DEBUGLOG("GOTO DIST: ", travelDist);
        DEBUGLOG("GOTO DRIVE PID: ", drivePower);
        DEBUGLOG("GOTO TURN PID: ", turnPower);

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

// Reverse Drive Movement
// Drives backward by a certain distance
bool AutonSystem::reverseDrive(double distance) {
    if (!odometrySystemPointer->isTracking) {
        brainError("Skipping Auton Path, Odom not initialized");
        return false;
    }
    bool wasrunning = running;
    running = true;

    odom::Position movementStartPos = odometrySystemPointer->currentPos();
    double deltaDist = distBetweenPoints(movementStartPos, odometrySystemPointer->currentPos());
    
    RightDriveSmart.spin(directionType::fwd, -5, voltageUnits::volt);
    LeftDriveSmart.spin(directionType::fwd, -5, voltageUnits::volt);

    while (deltaDist < distance) { deltaDist = distBetweenPoints(movementStartPos, odometrySystemPointer->currentPos()); wait(10, timeUnits::msec); }

    LeftDriveSmart.spin(directionType::fwd, 0, volt);
    RightDriveSmart.spin(directionType::fwd, 0, volt);

    running = wasrunning;
    return true;
}


// Pickup the acorn using the vision sensor to center it
bool AutonSystem::pickupAcorn() {

    if (!odometrySystemPointer->isTracking) {
        brainError("Skipping Auton Path, Odom not initialized");
        return false;
    }
    if (!visionSensor.installed()) {
        brainError("Vision Sensor not initialized");
        return false;
    }
    bool wasrunning = running;
    running = true;

    frontArmHolder.setNewVal(0);

    pid::PID turnPid(pid::PIDConfig(0.15, 0.00, 0.00), 50);
    turnPid.setMax(12);
    turnPid.setMin(-12);

    while (true) {
        visionSensor.takeSnapshot(SIG_1);
        double centerPercent = (double)visionSensor.largestObject.centerX / 350.00;
        double turnPower = turnPid.iterate(centerPercent * 100);

        LeftDriveSmart.spin(directionType::fwd, -turnPower + 4, volt);
        RightDriveSmart.spin(directionType::fwd, turnPower + 4, volt);

        DEBUGLOG("Acorn Pid Power: ", turnPower);
        DEBUGLOG("DIST: ", visionSensor.largestObject.width);

        if (visionSensor.largestObject.width > 265) { break; }

        wait(0.05, seconds);
    }

    LeftDriveSmart.spin(directionType::fwd, 0, volt);
    RightDriveSmart.spin(directionType::fwd, 0, volt);

    // Pickup Acorn Logic
    frontArmHolder.setRunning(true);
    frontArmHolder.setNewVal(100);

    wait(0.5, seconds);

    LeftDriveSmart.spin(directionType::fwd, -4, volt);
    RightDriveSmart.spin(directionType::fwd, -4, volt);

    wait(0.5, seconds);

    frontArmHolder.setRunning(false);

    LeftDriveSmart.spin(directionType::fwd, 0, volt);
    RightDriveSmart.spin(directionType::fwd, 0, volt);

    running = wasrunning;
    return true;

};
bool AutonSystem::dropAcorn() {

    if (!odometrySystemPointer->isTracking) {
        brainError("Skipping Auton Path, Odom not initialized");
        return false;
    }
    bool wasrunning = running;
    running = true;

    // Drop Acorn Logic
    frontArmHolder.setRunning(true);
    frontArmHolder.setNewVal(0);

    wait(0.5, seconds);

    frontArmHolder.setRunning(false);    

    running = wasrunning;
    return true;
}






void aiQueueSystem::addPtrs(AutonSystem* botAIPtr, odom::OdometrySystem* odometryPointer) {
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
    loaded = false;
    for (int i = 0; i < path.getSize(); i++) {
        queue.push_back(path.getStep(i));
    }
    aiPtr->setStartPos(odom::posToTilePos(path.startPos));
    loaded = true;
    return true;
}
bool aiQueueSystem::addToQueue(autonMovement movement) {
    loaded = false;
    queue.push_back(movement);
    loaded = true;
    return true;
}
bool aiQueueSystem::addToQueue(std::string jsonPath) {
    return addToQueue(getPathFromJSON(jsonPath));
}
autonPath aiQueueSystem::getPathFromJSON(std::string jsonPath) {

    DynamicJsonDocument* pathPtr = misc::readJsonFromFile(jsonPath.c_str());
    //std::cout << *pathPtr << std::endl;

    JsonObject path = pathPtr->as<JsonObject>();

    if (path.isNull()) {
        //delete path; // Free Memory
        return autonPath();
    }

    JsonArray movementsArray = path["movements"].as<JsonArray>();

    autonPath readPath = autonPath();

    JsonObject startPos = path["startPos"].as<JsonObject>();
    readPath.startPos = odom::tilePosToPos(odom::TilePosition(startPos["x"].as<float>(), startPos["y"].as<float>(), startPos["rot"].as<float>()));

    int length = path["length"].as<int>();
    //std::cout << length << std::endl;

    // Loop through all objects in the "movements" array
    for (const auto& movement : movementsArray) {
        autonMovement tmpMove;
        //std::cout << std::endl << std::endl;
        // Main stuff
        tmpMove.movementType = movement["type"].as<int>();
        tmpMove.val = movement["val"].as<float>();
        tmpMove.tilePosBool = movement["tilePosBool"].as<bool>();

        // Get Position Data
        JsonObject pos = movement["pos"].as<JsonObject>();
        if (pos["rot"].as<float>() == -1) {
            tmpMove.pos = odom::Position(pos["x"].as<float>(), pos["y"].as<float>());
        } else {
            tmpMove.pos = odom::Position(pos["x"].as<float>(), pos["y"].as<float>(), pos["rot"].as<float>());
        }

        // Get Tile Position Data
        JsonObject tilePos = movement["tilePosition"].as<JsonObject>();
        if (tilePos["rot"].as<float>() == -1) {
            tmpMove.tilePos = odom::TilePosition(tilePos["x"].as<float>(), tilePos["y"].as<float>());
        } else {
            tmpMove.tilePos = odom::TilePosition(tilePos["x"].as<float>(), tilePos["y"].as<float>(), tilePos["rot"].as<float>());
        }

        // Loop Through Drive Path Array
        JsonArray drivePathArray = movement["drivePath"].as<JsonArray>();

        for (const auto& point : drivePathArray) {
            if (point["rot"].as<float>() == -1) {
                tmpMove.drivePath.push_back(odom::TilePosition(point["x"].as<float>(), point["y"].as<float>()));
            } else {
                tmpMove.drivePath.push_back(odom::TilePosition(point["x"].as<float>(), point["y"].as<float>(), point["rot"].as<float>()));    
            }
        }

        readPath.addMovement(tmpMove);
    }

    DEBUGLOG("Finished Reading: ", jsonPath);
    DEBUGLOG("Parsed Size: ", readPath.getSize());
    DEBUGLOG("Read Size: ", length);

    if (length != readPath.getSize()) {
        brainError("Error Reading Json", true);
    }

    delete pathPtr; // Free Memory
    return readPath;
};
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
        case AUTON_MOVE_DRIVE_REVERSE:
            return aiPtr->reverseDrive(movement.val);
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