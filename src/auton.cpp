
#include "auton.h"
#include "robotConfig.h"
#include "paths.h"
#include <vector>
#include <cmath>


#include "visionSensorConfig.h"
#include "motionProfiling.h"


// Auton Specific Macros
// Check if ForceStop is set to true
#define CheckForceStop() if (this->forceStop) {  DEBUGLOG("Stopping: Forced Stop"); return false; }
#define CheckOdomStatus() if (!odometrySystemPointer->isTracking) { brainError("Skipping Auton Path, Odom not initialized"); return false; }

using namespace vex;
using namespace auton;



AutonSystem::AutonSystem(odom::OdometrySystem* systemPointer, aiQueueSystem* queuePtr) {
    odometrySystemPointer = systemPointer;
    queueSystemPtr = queuePtr;
    queuePtr->addPtrs(this, systemPointer);

    // Define the Config
    configStorage = {
        autonConfig("startSide", "Left", "Right", false, vex::color(50, 50, 50), vex::color(0, 0, 0)),
        autonConfig("isFinals", "Finals", false),
        autonConfig("isSkills", "Match", "Skills", false),
        autonConfig("isDisabled", "Is Disabled", false),
        autonConfig("teamColor", "Red", "Blue", false, vex::color(247, 30, 54), vex::color(62, 133, 247))
    };    
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
        queueSystemPtr->clear();

        // queueSystemPtr->addToQueue(buildPath(AUTON_PATH_TEST));
        return;
    }

    odometrySystemPointer->restart();

    runningSkills = getConfig("isSkills");
    runningFinals = getConfig("isFinals");

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
    
    if (botAI.getConfig("isDisabled")) {
        mainControllerMessage("Auton Disabled", 5);
    }

    // New Path System
    bool result = false;

    bool isFinals = getConfig("isFinals");
    std::string message = "Running ";

    if (runningSkills) {
        // result = queueSystemPtr->addToQueue(AUTON_PATH_FOLDER + AUTON_PATH_SKILLS_JSON);

        message += "Skills ";
        result = queueSystemPtr->addToQueue(buildPath(AUTON_PATH_SKILLS, isFinals));

    } else {
        if (!getConfig("startSide")) {
            // result = queueSystemPtr->addToQueue(AUTON_PATH_FOLDER + AUTON_PATH_LEFT_JSON);
            result = queueSystemPtr->addToQueue(buildPath(AUTON_PATH_LEFT, isFinals));
            message += "Left ";
        } else {
            //result = queueSystemPtr->addToQueue(AUTON_PATH_FOLDER + AUTON_PATH_RIGHT_JSON);
            result = queueSystemPtr->addToQueue(buildPath(AUTON_PATH_RIGHT, isFinals));
            message += "Right ";
        }
    }
    if (!result) {
        brainError("Error Reading JSON");
    } else {
        if (isFinals) {
            message += "Finals";
        }
        mainControllerMessage(message, 5);
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

bool AutonSystem::isFinals() { return runningFinals; };

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
bool AutonSystem::driveDist(double dist, bool precise) {
    odom::Position currentPosition = odometrySystemPointer->currentPos();
    double desiredX = currentPosition.x + dist * cos(2.5*PI - currentPosition.rot);
    double desiredY = currentPosition.y + dist * sin(2.5*PI - currentPosition.rot);
    return gotoLoc(odom::Position(desiredX, desiredY, NAN), precise);
}
// Turns to the desired rotation in degrees
bool AutonSystem::turnTo(double deg) {
    return turnTo(deg, 2);
}
// Turns to the desired rotation in degrees
bool AutonSystem::turnTo(double deg, double turnTimeout) {
    
    CheckOdomStatus();
    CheckForceStop();

    double timeout = Brain.timer(msec) + (turnTimeout * 1000);

    bool wasRunning = running;
    running = true;

    double heading = misc::radToDegree(odometrySystemPointer->currentPos().rot);
    double target = findNearestRot(heading, deg);
    pid::PID turnPID(pid::PIDConfig(0.12, 0.0, 0.0), target);
    turnPID.addBrainPtr(&Brain);
    turnPID.setIntegralLimit(-12, 12);

    // double accuracy = 3;
    int checks = 10;

    double lastRot = odometrySystemPointer->currentPos().rot;
    int totalChecks = 0;

    setMotors(0, 0, voltageUnits::volt);

    while (true) {
        CheckForceStop();
        
        heading = misc::radToDegree(odometrySystemPointer->currentPos().rot);
        double power = turnPID.iterate(heading);

        setMotors(-power, power, voltageUnits::volt);

        // DEBUGLOG("TURNTO PID: ", power);

        double headingError = target - heading;
        double deltaHeading = heading - lastRot;
        if (fabs(headingError) <= 3 || fabs(deltaHeading) < 2) {
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

    setMotors(0, 0, voltageUnits::volt);

    running = wasRunning;
    return true;
};

bool AutonSystem::gotoLoc(odom::TilePosition pos, bool precise) {
    return gotoLoc(odom::Position(pos), precise);
};


double voltFromDist(double dist) {
    return (atan((dist / 4.00) - 2.5) * (2 / PI) * 7) + 5.5;
}
double turnScaleFromDeg(double deltaDeg) {
    return ((-atan((deltaDeg / 4.00) - 5) * (2 / PI)) + 1) / 2;
}

// Motion Profiling System
bool AutonSystem::gotoLoc(odom::Position pos, bool precise) {
    
    CheckOdomStatus();
    CheckForceStop();

    bool wasRunning = running;
    running = true;
    
    target = pos;
    
    DEBUGLOG("Target: ", pos)

    odom::Position currentPos = odometrySystemPointer->currentPos();

    double travelDist = distBetweenPoints(currentPos, pos);
    double desiredHeading = misc::radToDegree(angleBetweenPoints(currentPos, pos));

    // Turn to point to the location
    // Only do it if the robot has to turn more than 10 degrees
    if (!(fabs(misc::radToDegree(currentPos.rot) - findNearestRot(misc::radToDegree(currentPos.rot), desiredHeading)) <= 20)) {
        turnTo(desiredHeading, 1);
    }
    double drivePower = 0.00;

    // PID to keep the robot driving straight
    pid::PID turnPid(pid::PIDConfig(0.2, 0.0, 0.0));
    turnPid.setMax(10);
    turnPid.setMin(-10);
    turnPid.addBrainPtr(&Brain);
    double turnPower = 0.00;

    pid::PIDConfig driveConfig(0.0, 0.0, 0.0);
    if (precise) {
        driveConfig = pid::PIDConfig(0.3, 0.0, 0.0);
    } else {
        driveConfig = pid::PIDConfig(0.75, 0.0, 0.0);
    }
    pid::PID drivePid(driveConfig);
    drivePid.setMax(11);
    drivePid.setMin(-11);
    odom::Position lastPos = odom::Position(0,0,0);

    bool traveling = true;
    int checks = 7;

    // double startTime = Brain.Timer.system();

    setMotors(0, 0, voltageUnits::volt);

    // Main Driving Loop
    while (traveling) {
        CheckForceStop();
        
        currentPos = odometrySystemPointer->currentPos();

        travelDist = distBetweenPoints(currentPos, pos);
        desiredHeading = misc::radToDegree(angleBetweenPoints(currentPos, pos));

        drivePower = drivePid.iterate(travelDist, 0);

        double turnCurrent = misc::radToDegree(odometrySystemPointer->currentPos().rot);
        double turnWant = findNearestRot(misc::radToDegree(currentPos.rot), desiredHeading);
        turnPower = turnPid.iterate(turnCurrent, turnWant);

        // Calculate Desired Velocities
        double leftVelocity = drivePower - turnPower;
        double rightVelocity = drivePower + turnPower;

        // Apply Velocities
        setMotors(leftVelocity, rightVelocity, voltageUnits::volt);

        if (travelDist < 1 || distBetweenPoints(lastPos, currentPos) < 0.5) {
            checks--;
            if (checks < 0) {
                traveling = false;
            }
        }

        lastPos = currentPos;

        wait(0.05, seconds);
    }

    // wait(0.1, seconds);

    if (!std::isnan(pos.rot)) {
        turnTo(pos.rot, 1.5);
    }

    wait(0.1, seconds);

    running = wasRunning;
    return true;
};

bool AutonSystem::longGoto(std::vector<odom::TilePosition> pos) {
    std::vector<odom::Position> tmpVec;
    for (int i = 0; i < pos.size(); i++) {
        tmpVec.push_back(odom::tilePosToPos(pos.at(i)));
    }
    return longGoto(tmpVec);
};
// Old System
bool AutonSystem::longGoto(std::vector<odom::Position> pos) {

    CheckOdomStatus();
    CheckForceStop();

    bool wasRunning = running;
    running = true;
    
    target = pos.at(0);

    odom::Position currentPos = odometrySystemPointer->currentPos();

    double travelDist = distBetweenPoints(currentPos, pos.at(0));
    double desiredHeading = misc::radToDegree(angleBetweenPoints(currentPos, pos.at(0)));

    // Turn to point to the location
    // Only do it if the robot has to turn more than 10 degrees
    if (!(fabs(misc::radToDegree(currentPos.rot) - findNearestRot(misc::radToDegree(currentPos.rot), desiredHeading)) <= 10)) {
        turnTo(desiredHeading, 1.5);
    }

    // PID to control drive speed
    pid::PID drivePid(pid::PIDConfig(0.5, 0.01, 0.0), 0);
    drivePid.setMax(11);
    drivePid.setMin(-11);
    double drivePower = 0.00;

    // PID to keep the robot driving straight
    pid::PID turnPid(pid::PIDConfig(0.05, 0.0, 0.0));
    turnPid.setMax(12);
    turnPid.setMin(-12);
    turnPid.addBrainPtr(&Brain);
    double turnPower = 0.00;

    misc::ValueAverager<10> turnWantAvg = misc::ValueAverager<10>();

    bool traveling = true;

    int targetIndex = 0;
    int numOfTargets = pos.size();

    int counter = 0;

    // Main Driving Loop
    while (traveling) {
        CheckForceStop();

        currentPos = odometrySystemPointer->currentPos();

        travelDist = distBetweenPoints(currentPos, pos.at(targetIndex));
        
        // if (targetIndex < numOfTargets - 1) {
            //travelDist += distBetweenPoints(pos.at(targetIndex), pos.at(targetIndex + 1)) / 2;
        // }

        desiredHeading = misc::radToDegree(angleBetweenPoints(currentPos, pos.at(targetIndex)));

        double turnCurrent = misc::radToDegree(odometrySystemPointer->currentPos().rot);
        double turnWant = turnWantAvg.iterate(
            findNearestRot(misc::radToDegree(currentPos.rot), desiredHeading)
        );
        turnPower = turnPid.iterate(turnCurrent, turnWant);

    
        // drivePower = drivePid.iterate(travelDist);
        drivePower = 8.5;

        double turnError = fabs(turnWant - turnCurrent);
        drivePower = drivePower * pow(0.98, turnError);

        counter ++;
        if (counter % 10 == 0) {
            // DEBUGLOG(centerPower);
        }

        

        // double leftPower = - centerPower;
        // double rightPower =  centerPower;

        double leftPower = drivePower - turnPower;
        double rightPower = drivePower + turnPower;

        setMotors(leftPower, rightPower, voltageUnits::volt);

        // If robot is close to the point, increase point index
        // If done with path, exit loop
        if (travelDist < 6) {
            if (targetIndex < numOfTargets - 1) {
                targetIndex++;
                target = pos.at(targetIndex);
            } else {
                
                if (travelDist < 6) {
                    traveling = false;
                }

            }
        }

        wait(0.05, seconds);
    }

    setMotors(0, 0, voltageUnits::volt);
    

    if (!std::isnan(pos.at(pos.size() - 1).rot)) {
        turnTo(pos.at(pos.size() - 1).rot, 2);
    }
    
    running = wasRunning;
    return true;
};

// Reverse Drive Movement
// Drives backward by a certain distance
bool AutonSystem::reverseDrive(double distance) {
    CheckOdomStatus();
    CheckForceStop();

    bool wasrunning = running;
    running = true;

    odom::Position movementStartPos = odometrySystemPointer->currentPos();
    double deltaDist = distBetweenPoints(movementStartPos, odometrySystemPointer->currentPos());

    pid::PID drivePid = pid::PID(pid::PIDConfig(0.55, 0.0, 0), distance);

    setMotors(0, 0, voltageUnits::volt);

    double endTime = Brain.timer(timeUnits::msec) + 500;

    int checks = 0;
    bool running = true;
    while (running) { 
        CheckForceStop();
        deltaDist = distBetweenPoints(movementStartPos, odometrySystemPointer->currentPos()); 

        double power = drivePid.iterate(deltaDist);
        setMotors(power, power, voltageUnits::volt);

        if (
            (fabs(odometrySystemPointer->getVelocity()) < 1 && Brain.timer(timeUnits::msec) > endTime)
        ) {
            checks ++;
        }

        if (checks > 10) {
            running = false;
        }

        wait(10, timeUnits::msec); 
    }

    setMotors(0, 0, voltageUnits::volt);

    running = wasrunning;
    return true;
}

bool AutonSystem::catapult(int launchTime) {
    CheckOdomStatus();

    startCatapult();

    vex::wait(launchTime, vex::timeUnits::sec);

    stopCatapult();

    return true;
};




// Queuing System Functions

#include "libs/spline.h"
void aiQueueSystem::addPtrs(AutonSystem* botAIPtr, odom::OdometrySystem* odometryPointer) {
    aiPtr = botAIPtr;
    odomPtr = odometryPointer;
};
void aiQueueSystem::runQueue() {
    if (running) { return; }
    running = true;
    aiPtr->setForceStop(false);

    int i = 0;
    while (i < queue.size()) {
        runMovement(queue.at(i)); 
        i++;
    }

    queue.clear();
    running = false;
    aiPtr->setForceStop(true);

}
void aiQueueSystem::clear() {
    queue.clear();
}
std::vector<autonMovement> aiQueueSystem::getQueue() {
    return queue;
};
bool aiQueueSystem::addToQueue(autonPath path) {
    loaded = false;
    for (int i = 0; i < path.size(); i++) {
        this->addToQueue(path[i]); // Extract the movements from the path
    }
    aiPtr->setStartPos(odom::posToTilePos(path.startPos)); // Set the starting pos
    loaded = true;
    return true;
}
bool aiQueueSystem::addToQueue(autonMovement movement) {
    loaded = false;
    if (movement.movementType == AUTON_MOVE_LONGGOTO && movement.drivePath.size() > 2) {

        int resolution = 2;
        double timeBetweenPoints = 2;

        std::vector<double> timePoses;
        std::vector<double> xPoses;
        std::vector<double> yPoses;
        for (int i = 0; i < movement.drivePath.size(); i++) {
            timePoses.push_back(i * timeBetweenPoints);
            xPoses.push_back(movement.drivePath.at(i).x);
            yPoses.push_back(movement.drivePath.at(i).y);
        }

        tk::spline xSpline(timePoses, xPoses);
        tk::spline ySpline(timePoses, yPoses);

        
        std::vector<odom::TilePosition> pos;
        for (int i = 0; i < ((movement.drivePath.size() - 1) * resolution) + 1; i++) {
            pos.push_back(
                odom::TilePosition(
                        xSpline(i * (timeBetweenPoints / resolution)), 
                        ySpline(i * (timeBetweenPoints / resolution)),
                        movement.drivePath.at(movement.drivePath.size() -1).rot
                    )
                );
        }

        movement.drivePath.clear();
        movement.drivePath = pos;

    }

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

        readPath.add(tmpMove);
    }

    DEBUGLOG("Finished Reading: ", jsonPath);
    DEBUGLOG("Parsed Size: ", readPath.size());
    DEBUGLOG("Read Size: ", length);

    if (length != readPath.size()) {
        brainError("Error Reading Json", true);
    }

    delete pathPtr; // Free Memory
    return readPath;
};
bool aiQueueSystem::runMovement(autonMovement movement) {
    DEBUGLOG("");
    
    switch (movement.movementType) {

        case AUTON_MOVE_DELAY:
            DEBUGLOG("AUTO RUN: Delay");
            wait(movement.val, timeUnits::msec);
            return true;

        case AUTON_MOVE_DRIVE_DIST:
            DEBUGLOG("AUTO RUN: Drive Dist");
            return aiPtr->driveDist(movement.val, false);

        case AUTON_MOVE_SLOW_DRIVE_DIST:
            DEBUGLOG("AUTO RUN: Drive Dist");
            return aiPtr->driveDist(movement.val, true);

        case AUTON_MOVE_GOTO:
            DEBUGLOG("AUTO RUN: Goto");
            if (movement.tilePosBool) {
                return aiPtr->gotoLoc(movement.tilePos, false);
            } else {
                return aiPtr->gotoLoc(movement.pos, false);
            }
        case AUTON_MOVE_SLOW_GOTO:
            DEBUGLOG("AUTO RUN: Goto - slow");
            if (movement.tilePosBool) {
                return aiPtr->gotoLoc(movement.tilePos, true);
            } else {
                return aiPtr->gotoLoc(movement.pos, true);
            }

        case AUTON_MOVE_LONGGOTO:
            DEBUGLOG("AUTO RUN: Long Goto");
            return aiPtr->longGoto(movement.drivePath);

        case AUTON_MOVE_DRIVE_REVERSE:
            DEBUGLOG("AUTO RUN: Drive Reverse");
            return aiPtr->reverseDrive(movement.val);

        case AUTON_MOVE_TURNTO:
            DEBUGLOG("AUTO RUN: Turn To");
            return aiPtr->turnTo(movement.val, 1);

        case AUTON_MOVE_SLOW_TURNTO:
            DEBUGLOG("AUTO RUN: Turn To - slow");
            return aiPtr->turnTo(movement.val, 2);

        case AUTON_MOVE_CATAPULT:
            DEBUGLOG("AUTO RUN: Catapult");
            return aiPtr->catapult(movement.val);

        case AUTON_MOVE_WING_SET_STATE:
            DEBUGLOG("AUTO RUN: Wing Set State");
            setWings((WingStates)movement.val);
            vex::wait(400, vex::timeUnits::msec);
            return true;
        
        case AUTON_MOVE_INTAKE_SET:
            DEBUGLOG("AUTO RUN: Intake Direction Set");
            setIntakeDir(movement.val);

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