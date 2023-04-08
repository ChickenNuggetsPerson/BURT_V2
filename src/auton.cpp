
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

    Position tempPos;
    tempPos.x = 10;
    tempPos.y = 20;
    tempPos.rot = 1*PI;

    return tempPos;
}

// Returns true if the autonomous is ready
bool ai::isReady() {return loaded;};

void ai::started() {
    brainFancyDebug("Auton Started", vex::color::cyan, true);
};

