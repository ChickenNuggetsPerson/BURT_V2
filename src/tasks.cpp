

// All of the tasks that run separately

#include "robotConfig.h"
#include "tasks.h"

using namespace vex;

void test(){
    brainFancyDebug("YEET %d", green, rand());
};

void rightPressed() {
 botAI.gotoLoc(TilePosition(4, 3, 0));
 //botAI.turnTo(90);
};
void leftPressed() {
  brainChangePage("map");
};
void upPressed() {
  //botAI.gotoLoc(TilePosition(1, 5, 0));
}
void downPressed() {
  //botAI.gotoLoc(TilePosition(0, 0, 0));
}

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


void autonomous(void) {
  botAI.started();
}


void usercontrol(void) {
  botAI.stop();
  brainFancyDebug("UserControl Started", vex::color::green, true);
}


void whenStarted(void){
  
}

