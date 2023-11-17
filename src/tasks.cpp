

// All of the tasks that run separately

#include "robotConfig.h"
#include "tasks.h"

using namespace vex;

// Debug Functions
void rightPressed() {
  //botAI.setForceStop(false);
  //botAI.driveDist(24*2); 
  //botAI.driveDist(24);
  //botAI.gotoLoc(botAI.getStartPos());
  //botAI.setForceStop(true);

};
void leftPressed() {
  while (true) {
    cataSystem.reset();
    cataSystem.launch();
    if (mainController.ButtonRight.pressing()) {
      break;
    }
  }
};
void upPressed() {
  //botAI.gotoLoc(botAI.getStartPos());
  
}
void downPressed() {
  while (true)
  {
    queuingSystem.clear();
    queuingSystem.addToQueue(auton::autonMovement(AUTON_MOVE_TURNTO, 90));
    queuingSystem.addToQueue(auton::autonMovement(AUTON_MOVE_TURNTO, -90));
    queuingSystem.runQueue();
  }
  
}

// Setup callbacks and initialize auton
void pre_auton(void) {

  // Imedietely stop the motors so then they don't spin when the robot boots up
  leftMotorA.spin(fwd);  leftMotorA.setVelocity(0, percent);
  leftMotorB.spin(fwd);  leftMotorB.setVelocity(0, percent);
  rightMotorA.spin(fwd); rightMotorA.setVelocity(0, percent);
  rightMotorB.spin(fwd); rightMotorB.setVelocity(0, percent);

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


void autonomous(void) { 
  queuingSystem.autonStarted(); 
}


void usercontrol(void) {
  brainFancyDebug("UserControl Started", vex::color::green, true);
  
  botAI.setForceStop(true);
  frontArmHolder.setRunning(false);
}

void whenStarted(void) {

  DEBUGLOG("BATTERY STATUS: ", Brain.Battery.capacity());

}