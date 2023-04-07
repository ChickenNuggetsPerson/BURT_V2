

// All of the tasks that run separately

#include "robotConfig.h"
#include "tasks.h"

using namespace vex;

void test(){
    brainFancyDebug("YEET %d", green, rand());
};

void rightPressed() {
  brainChangePage("main");
};
void leftPressed() {
  brainChangePage("config");
};

void pre_auton(void) {

  brainDebug("Initializing Burt");

  altController.ButtonDown.pressed(debugDisable);
  altController.ButtonLeft.pressed(debugAuton);
  altController.ButtonRight.pressed(debugDriver);

  altController.ButtonR2.pressed(test);

  mainController.ButtonRight.pressed(rightPressed);
  mainController.ButtonLeft.pressed(leftPressed);

  startTasks();
  botAI.init();
}


void autonomous(void) {
  botAI.started();
}


void usercontrol(void) {
  brainFancyDebug("UserControl Started", vex::color::green);
}


void whenStarted(void){
  
}

