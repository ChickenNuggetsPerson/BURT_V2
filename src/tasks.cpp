

// All of the tasks that run separately

#include "robotConfig.h"
#include "tasks.h"

using namespace vex;

// Debug Functions
void rightPressed() {
  queuingSystem.addToQueue(autonMovement(AUTON_MOVE_GOTO, botAI.getStartPos()));
};
void leftPressed() {
  queuingSystem.runQueue();
};
void upPressed() {

}
void downPressed() {
  
}

// Setup callbacks and initialize auton
void pre_auton(void) {

  brainDebug("Initializing Burt", true);

  altController.ButtonDown.pressed(debugDisable);
  altController.ButtonLeft.pressed(debugAuton);
  altController.ButtonRight.pressed(debugDriver);

  mainController.ButtonRight.pressed(rightPressed);
  mainController.ButtonLeft.pressed(leftPressed);
  mainController.ButtonUp.pressed(upPressed);
  mainController.ButtonDown.pressed(downPressed);

  startTasks();
  inertialSensor.startCalibration(); // Start the calibration right away
  botAI.init();
}


void autonomous(void) { queuingSystem.autonStarted(); }


void usercontrol(void) {
  brainFancyDebug("UserControl Started", vex::color::green, true);
}

void whenStarted(void) { }