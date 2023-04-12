
#include "auton.h"
#include "robotConfig.h"

using namespace vex;

ai::ai(OdometrySystem* systemPointer) {
    odometrySystemPointer = systemPointer;
}

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

            //std::cout << (configFoler + configNames[i]).c_str() << " " << configStorage[i] << " " << valueRead << std::endl; 
            wait(0.1, seconds);
        }
        loaded = true;
        brainFancyDebug("Auton Initialized", cyan, true);
        odometrySystemPointer->restart(getStartPos());
    } else {
        brainError("No SD Card");
        odometrySystemPointer->restart();
    }

    
};



bool ai::getConfig(const char* configName) {
    for (int i = 0; i < totalConfigs; i++) {
        if (configNames[i] == std::string(configName)) {
            return configStorage[i];
        }
    }
    return false;
};
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

Position ai::getStartPos() {

    // TODO: Once game released, determin starting positions and make a lookup based on config 

    // Other Idea: if the robot starts against the wall, use the distance sensor to measure distance
    //             from perpendicular wall, which will provide a more accurate starting position

    if (!Brain.SDcard.isInserted()) { return Position(); }

    Position tempPos = Position(10, 20, 90);

    return tempPos;
}

// Returns true if the autonomous is ready
bool ai::isReady() {return loaded;};

// Finds the nearest rotation based on the provided target absolute rotation
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


bool ai::turnTo(double deg) {

    if (running) { return false; }

    running = true;

    double heading = radToDegree(odometrySystemPointer->currentPos().rot);
    double target = findNearestRot(heading, deg);
    PID turnPID(PIDConfig(0.1, 0, 0), target);


    double accuracy = 0.75;
    int checks = 7;


    int totalChecks = 0;
    while (true) {
        
        heading = radToDegree(odometrySystemPointer->currentPos().rot);
        double power = turnPID.iterate(heading);

        std::cout << power << " " << limitAngle(heading) << std::endl;

        LeftDriveSmart.spin(fwd, -power, volt);
        RightDriveSmart.spin(fwd, power, volt);
    
        wait(0.05, seconds);

        if (power <= accuracy && power >= -accuracy) {
            totalChecks++;
            if (totalChecks > checks) {
                break;
            }
        }
    }


    PID correctPID(PIDConfig(0.5, 0, 0.25), target);
    accuracy = 1;
    checks = 7;

    totalChecks = 0;
    std::cout << "starting accurate" << std::endl;

    while (true) {
        
        heading = radToDegree(odometrySystemPointer->currentPos().rot);
        double power = correctPID.iterate(heading);

        std::cout << power << " " << limitAngle(heading) << std::endl;

        LeftDriveSmart.spin(fwd, -power, volt);
        RightDriveSmart.spin(fwd, power, volt);
    
        wait(0.05, seconds);

        if (heading < target + 1 && heading > target - 1) {
            break;
        }
        totalChecks++;
        if (totalChecks > checks) {
            break;
        }
    }


    LeftDriveSmart.spin(fwd, 0, volt);
    RightDriveSmart.spin(fwd, 0, volt);

    running = false;
    return true;
};

bool ai::gotoLoc(TilePosition pos) {return gotoLoc(odometrySystemPointer->tilePosToPos(pos));};
bool ai::gotoLoc(Position pos) {

    // Required: Odomotry system needs to be working to do this

    // TODO:
    //  1. Figure out the math for finding the angle and distance to the point
    //  2. Develop a "stright drive" function that uses the inertial sensor and track wheel to drive stright
    //  3  Compare desired position to the final position and correct

    Position currentPos = odometrySystemPointer->currentPos();
    double distX = pos.x - currentPos.x;
    double distY = pos.y - currentPos.y;

    double desiredHeading = radToDegree(atan(distX / distY));

    std::cout << desiredHeading << std::endl;
    
    turnTo(desiredHeading);

    return true;
};


void ai::started() {
    brainFancyDebug("Auton Started", vex::color::cyan, true);
};


