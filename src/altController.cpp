
#include "robotConfig.h"
#include "altController.h"

using namespace vex;


void altControllerRender() {
    altController.Screen.clearScreen();
    altController.Screen.setCursor(1, 1);
    altController.Screen.print("Alt Controller");
    altController.Screen.setCursor(2, 1);
    altController.Screen.print("Connected to: ");
    altController.Screen.setCursor(3, 1);

    if (Competition.isFieldControl()) {
        altController.Screen.print("Field");
    }
    if (Competition.isCompetitionSwitch()) {
        altController.Screen.print("Competition Switch");
    }
    if (!Competition.isFieldControl() && !Competition.isCompetitionSwitch()) {
        altController.Screen.print("Nothing");
    };
};

void debugDisable() { Competition.test_disable(); brainError("Driving Disabled");};
void debugDriver() {  Competition.test_driver();  };
void debugAuton() {   Competition.test_auton();   };

