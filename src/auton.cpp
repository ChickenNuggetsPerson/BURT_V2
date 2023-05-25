
#include "auton.h"
#include "robotConfig.h"

using namespace vex;

ai::ai(OdometrySystem* systemPointer) {
    odometrySystemPointer = systemPointer;
}

// Load Auton Configs
void ai::init() {

    if (loaded) { 
        // Reset auton config
        loaded = false;
        for (int i = 0; i < totalConfigs; i++) {
            configStorage[i] = false;
        }
        brainFancyDebug("Auton Reset", blue, true);
    }

    // Load auton Config
    if (Brain.SDcard.isInserted()) {
        for (int i = 0; i < totalConfigs; i++) {
            unsigned int valueRead = readFile((configFoler + configNames[i] + configFileType).c_str());
            if ( valueRead == 1) {
                configStorage[i] = true;
            } else {
                configStorage[i] = false;
            } 
        }
        wait(0.1, seconds);
        loaded = true;
        brainFancyDebug("Auton Initialized", cyan, true);
        odometrySystemPointer->restart(getStartPos());
    } else {
        brainError("No SD Card");
        odometrySystemPointer->restart();
        return;
    }

    // Initalize Auton Variables

    if (getConfig("Red")) {
        teamColor = TEAM_RED;
    }
    if (getConfig("Blue")) {
        teamColor = TEAM_BLUE;
    }

    
};


// Return the config value based on the name
bool ai::getConfig(const char* configName) {
    for (int i = 0; i < totalConfigs; i++) {
        if (configNames[i] == std::string(configName)) {
            return configStorage[i];
        }
    }
    return false;
};

// Saves the value to the sd card
// Does not update internal storage so make sure to reintialize auton after writing configs
void ai::saveConfig(const char* configName, bool value) {

        std::cout << "Saving: " << configName << " " << value << std::endl;

        for (int i = 0; i < totalConfigs; i++) {
        if (configNames[i] == std::string(configName)) {
            int writeVal = 0;
            if (value) { writeVal= 1;}
            writeFile((configFoler + configNames[i] + configFileType).c_str(), writeVal);
        }
    }

};

// Returns the auton start position based on the config
Position ai::getStartPos() {


    // Idea: if the robot starts against the wall, use the distance sensor to measure distance
    //           from perpendicular wall, which will provide a more accurate starting position

    if (!Brain.SDcard.isInserted()) { return Position(); }

    bool left = getConfig("Left");
    bool right = getConfig("Right"); 

    if (left) {
        return odometrySystemPointer->tilePosToPos(AUTON_START_LEFT);
    } 
    if (right) {
        return odometrySystemPointer->tilePosToPos(AUTON_START_RIGHT);
    }

    return odometrySystemPointer->tilePosToPos(TilePosition(0, 0, 0));
    
}

// Returns true if the autonomous is ready
bool ai::isReady() {return loaded;};

// Finds the nearest rotation based on the provided target absolute rotation
// USES DEGREES AND NOT RADIANS
double ai::findNearestRot(double currentRot, double targetRot) {

    int currentMult = trunc(currentRot / 360);

    double smallerVal = (( currentMult ) * 360) + targetRot;
    double biggerVal = (( currentMult + 1 ) * 360) + targetRot;

    double smallerDiff = fabs(smallerVal - currentRot);
    double biggerDiff = fabs(biggerVal - currentRot);

    if (smallerDiff < biggerDiff) {
        return smallerVal;
    } else {
        return biggerVal;
    }
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


// Turns to the desired rotation in degrees
bool ai::turnTo(double deg) {
    return turnTo(deg, 3);
}
// Turns to the desired rotation in degrees
bool ai::turnTo(double deg, double turnTimeout) {
    
    if (!odometrySystemPointer->isTracking) { 
        brainError("Skipping Auton Path, Odom not initialized");
        return false; 
    }

    double timeout = Brain.timer(msec) + (3 * 1000);

    bool wasRunning = running;
    running = true;

    double heading = radToDegree(odometrySystemPointer->currentPos().rot);
    double target = findNearestRot(heading, deg);
    PID turnPID(AUTON_TURNTO_PID_CONFIG, target);


    double accuracy = 0.35;
    int checks = 20;


    int totalChecks = 0;
    while (true) {
        
        heading = radToDegree(odometrySystemPointer->currentPos().rot);
        double power = turnPID.iterate(heading);

        //std::cout << power << " " << limitAngle(heading) << std::endl;

        LeftDriveSmart.spin(fwd, -power, volt);
        RightDriveSmart.spin(fwd, power, volt);

        if (power <= accuracy && power >= -accuracy) {
            totalChecks++;
            if (totalChecks > checks) {
                break;
            }
        }

        if (Brain.timer(msec) > timeout) {
            break;
        }

        
        wait(0.05, seconds);
    }

    LeftDriveSmart.spin(fwd, 0, volt);
    RightDriveSmart.spin(fwd, 0, volt);

    running = wasRunning;
    return true;
};



bool ai::gotoLoc(TilePosition pos) {return gotoLoc(odometrySystemPointer->tilePosToPos(pos));};
// Goes to the desired location
// Set a NAN rotation to skip final turnto rotation
bool ai::gotoLoc(Position pos) {

    if (!odometrySystemPointer->isTracking) { 
        brainError("Skipping Auton Path, Odom not initialized");
        return false; 
    }

    bool wasRunning = running;
    running = true;
    target.x = pos.x;
    target.y = pos.y;
    target.rot = pos.rot;

    std::cout << "Target: " << pos.x << " " << pos.y << std::endl;

    // Required: Odomotry system needs to be working to do this

    // TODO:
    //  1. Figure out the math for finding the angle and distance to the point
    //  2. Develop a "stright drive" function that uses the inertial sensor and track wheel to drive stright
    //  3  Compare desired position to the final position and correct

    Position currentPos = odometrySystemPointer->currentPos();

    double travelDist = distBetweenPoints(currentPos, pos);
    double desiredHeading = radToDegree(angleBetweenPoints(currentPos, pos));
    
    turnTo(desiredHeading, 2);

    // PID to control drive speed
    PID drivePid(AUTON_GOTO_DRIVE_PID_CONFIG, 0);
    double drivePower = 0.00;

    // PID to keep the robot driving straight
    PID turnPid(AUTON_GOTO_TURN_PID_CONFIG);
    double turnPower = 0.00;

    int stopped = 0;
    bool traveling = true;
    while (traveling) {

        
        Position tempPos = odometrySystemPointer->currentPos();

        travelDist = distBetweenPoints(tempPos, pos);
        desiredHeading = radToDegree(angleBetweenPoints(tempPos, pos));

        if (travelDist < 1) {
            traveling = false;
        }

        drivePower = drivePid.iterate(travelDist);

        double turnCurrent = radToDegree(odometrySystemPointer->currentPos().rot);
        double turnWant = findNearestRot(radToDegree(tempPos.rot), desiredHeading);
        turnPower = turnPid.iterate(turnCurrent, turnWant);

        double leftPower = drivePower - turnPower;
        double rightPower = drivePower + turnPower;

        LeftDriveSmart.spin(fwd, leftPower, voltageUnits::volt);
        RightDriveSmart.spin(fwd, rightPower, voltageUnits::volt);

        if (drivePower < 0.5) {
            stopped++;
        }
        if (stopped > 10) {
            traveling = false;
        }

        wait(0.05, seconds);
    }

    if (!isnan(pos.rot)) {
        turnTo(pos.rot, 1.5);
    }
    
    running = wasRunning;
    return true;
};

void ai::stop() {
    running = false;
}

// Run when the autonomous period is started
void ai::started() {
    brainFancyDebug("Auton Started", vex::color::cyan, true);
    brainChangePage("map");


    // Test Auton Start Code

    gotoLoc(TilePosition(1, 1));
    gotoLoc(TilePosition(0, 1));
    gotoLoc(TilePosition(0, 0));

};


