

// All of the tasks that run separately

#include "robotConfig.h"
#include "tasks.h"

using namespace vex;

void pre_auton(void) {

  altController.ButtonDown.pressed(debugDisable);
  altController.ButtonLeft.pressed(debugAuton);
  altController.ButtonRight.pressed(debugDriver);

  startTasks();
  botAI.init();
}


void autonomous(void) {
  brainDebugColor("Auton Started", vex::color::cyan);
}


void usercontrol(void) {
  brainDebugColor("UserControl Started", vex::color::green);
}


void whenStarted(void){
  
}

