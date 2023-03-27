
#include "robotConfig.h"
#include "altController.h"

using namespace vex;




void debugDisable() { Competition.test_disable(); brainError("Driving Disabled");};
void debugDriver() {  Competition.test_driver();  };
void debugAuton() {   Competition.test_auton();   };