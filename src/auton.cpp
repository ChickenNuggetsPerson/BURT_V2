
#include "auton.h"
#include "robotConfig.h"

using namespace vex;

void ai::init() {

    if (loaded) { 
        // Reset auton
        loaded = false;
        for (int i = 0; i < totalConfigs; i++) {
            configStorage[i] = false;
        }
        brainFancyDebug("Auton Reset", blue, true);
    }
    // Load auton

    if (Brain.SDcard.isInserted()) {
        for (int i = 0; i < totalConfigs; i++) {
            if (readFile((configFoler + configNames[i]).c_str()) == 1) {
                configStorage[i] = true;
            } else {
                configStorage[i] = false;
            }

            //std::cout << configNames[i] << " " << configStorage[i] << std::endl; 
            wait(0.1, seconds);
        }
        loaded = true;
        brainFancyDebug("Auton Initialized", cyan, true);
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
        for (int i = 0; i < totalConfigs; i++) {
        if (configNames[i] == std::string(configName)) {
            if (value) {
                writeFile((configFoler + configNames[i]).c_str(), 1);
            } else {
                writeFile((configFoler + configNames[i]).c_str(), 0);
            }
        }
    }

};


// Returns true if the autonomous is ready
bool ai::isReady() {return loaded;};

void ai::started() {
    brainFancyDebug("Auton Started", vex::color::cyan);
};

