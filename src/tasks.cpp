

// All of the tasks that run separately

#include "robotConfig.h"
#include "tasks.h"

using namespace vex;

// Debug Functions
void rightPressed() {
  botAI.gotoLoc(TilePosition(1, 4.75, NAN));
  botAI.gotoLoc(TilePosition(4, 4.75, NAN));
  botAI.gotoLoc(TilePosition(4, 0, NAN));
  botAI.gotoLoc(TilePosition(1, 0));
};
void leftPressed() {
  botAI.gotoLoc(botAI.getStartPos());
};
void upPressed() {

  std::vector<TilePosition> tmpPath;
  tmpPath.push_back(TilePosition(1, 1));
  tmpPath.push_back(TilePosition(2, 1));
  tmpPath.push_back(TilePosition(2, 0));
  tmpPath.push_back(TilePosition(1, 0));
  botAI.longGoto(tmpPath);
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


void autonomous(void) { botAI.started(); }


void usercontrol(void) {
  botAI.stop(); // Force auton to stop
  brainFancyDebug("UserControl Started", vex::color::green, true);
}

void whenStarted(void) { }