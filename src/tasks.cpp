

// All of the tasks that run separately

#include "robotConfig.h"
#include "tasks.h"

using namespace vex;

void test(){
    brainFancyDebug("YEET %d", green, rand());
};

void rightPressed() {

};
void leftPressed() {
  std::vector<TilePosition> path;
  
  path.push_back(TilePosition(0, 0));
  path.push_back(TilePosition(0, 5));
  path.push_back(TilePosition(0, 0));

  botAI.longGoto(path);
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

  altController.ButtonR2.pressed(test);

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


void whenStarted(void) {}