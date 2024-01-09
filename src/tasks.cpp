

// All of the tasks that run separately

#include "robotConfig.h"
#include "tasks.h"

using namespace vex;


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

  startTasks();
  inertialSensor.startCalibration(); // Start the calibration right away
  botAI.init();
}


void autonomous(void) { 

  if (botAI.getConfig("isDisabled")) {
    return;
  }

  if (!Brain.SDcard.isInserted()) {
    mainControllerMessage("SD Card not Inserted", 5);
    
    return;
  }

  queuingSystem.autonStarted(); 
}


void usercontrol(void) {
  brainFancyDebug("UserControl Started", vex::color::green, true);
  botAI.setForceStop(true);
}

void whenStarted(void) {

  DEBUGLOG("BATTERY STATUS: ", Brain.Battery.capacity());

}