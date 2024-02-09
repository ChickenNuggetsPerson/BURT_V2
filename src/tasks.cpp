

// All of the tasks that run separately

#include "robotConfig.h"
#include "tasks.h"

using namespace vex;


void rightPressed() {


  queuingSystem.clear();
  queuingSystem.addToQueue(auton::autonMovement(AUTON_MOVE_DRIVE_DIST, 24));
  queuingSystem.runQueue();

  return;

  botAI.setForceStop(false);
  botAI.running = true;

  leftMotorA.spin(fwd, 13, voltageUnits::volt);
  vex::wait(2, timeUnits::sec);
  leftMotorA.spin(fwd, 0, voltageUnits::volt);
  vex::wait(2, timeUnits::sec);

  leftMotorB.spin(fwd, 13, voltageUnits::volt);
  vex::wait(2, timeUnits::sec);
  leftMotorB.spin(fwd, 0, voltageUnits::volt);
  vex::wait(2, timeUnits::sec);

  leftMotorC.spin(fwd, 13, voltageUnits::volt);
  vex::wait(2, timeUnits::sec);
  leftMotorC.spin(fwd, 0, voltageUnits::volt);
  vex::wait(2, timeUnits::sec);

  rightMotorA.spin(fwd, 13, voltageUnits::volt);
  vex::wait(2, timeUnits::sec);
  rightMotorA.spin(fwd, 0, voltageUnits::volt);
  vex::wait(2, timeUnits::sec);

  rightMotorB.spin(fwd, 13, voltageUnits::volt);
  vex::wait(2, timeUnits::sec);
  rightMotorB.spin(fwd, 0, voltageUnits::volt);
  vex::wait(2, timeUnits::sec);

  rightMotorC.spin(fwd, 13, voltageUnits::volt);
  vex::wait(2, timeUnits::sec);
  rightMotorC.spin(fwd, 0, voltageUnits::volt);
  vex::wait(2, timeUnits::sec);

  botAI.running = false;
}

// Setup callbacks and initialize auton
void pre_auton(void) {

  // mainController.ButtonRight.pressed(rightPressed);

  // Imedietely stop the motors so then they don't spin when the robot boots up
  setMotors(0, 0, velocityUnits::pct);

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


