

// All of the tasks that run separately

#include "robotConfig.h"
#include "tasks.h"

using namespace vex;

void test(){
    brainFancyDebug("YEET %d", green, rand());
};

void pre_auton(void) {

  altController.ButtonDown.pressed(debugDisable);
  altController.ButtonLeft.pressed(debugAuton);
  altController.ButtonRight.pressed(debugDriver);

  altController.ButtonR2.pressed(test);

  startTasks();
  botAI.init();
}


void autonomous(void) {
  brainFancyDebug("Auton Started", vex::color::cyan);
}


void usercontrol(void) {
  brainFancyDebug("UserControl Started", vex::color::green);
}


void whenStarted(void){
  
}

