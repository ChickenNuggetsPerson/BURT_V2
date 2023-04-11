
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
        Odometry.restart(getStartPos());
    } else {
        brainError("No SD Card");
        Odometry.restart();
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

    if (!Brain.SDcard.isInserted()) { return Position(); }

    Position tempPos = Position(10, 20, 90);

    return tempPos;
}

// Returns true if the autonomous is ready
bool ai::isReady() {return loaded;};

void ai::started() {
    brainFancyDebug("Auton Started", vex::color::cyan, true);
};


void setMotors(double right, double left) {
    
}


bool ai::turnTo(double deg) {

    running = true;

    PID turnPID(PIDConfig(0.15, 0.001, 0.4), deg);

    double lastHeading = 0;
    bool correcting = false;

    double accuracy = 0.30;
    int checks = 15;


    int totalChecks = 0;
    while (true) {
        
        double heading = Odometry.currentPos().rot;

        if (lastHeading > 0 && lastHeading < 90 && heading > 270 && !correcting) {
            correcting = true;
        }

        if (heading > 0 && heading < 90 && correcting) {
            correcting = false;
        }

        if (correcting) {
            heading = 360.00 - heading;
        }


        double power = turnPID.iterate(heading);

        std::cout << power << " " << heading << std::endl;

        LeftDriveSmart.spin(fwd, -power, volt);
        RightDriveSmart.spin(fwd, power, volt);


        lastHeading = heading;
    
        wait(0.05, seconds);

        if (power <= accuracy && power >= -accuracy) {
            totalChecks++;
            if (totalChecks > checks) {
                break;
            }
        }
    }


    LeftDriveSmart.spin(fwd, 0, volt);
    RightDriveSmart.spin(fwd, 0, volt);


    running = false;
    return true;
};