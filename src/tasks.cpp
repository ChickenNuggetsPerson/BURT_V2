

// All of the tasks that run separately

#include "robotConfig.h"
#include "tasks.h"

using namespace vex;


void rightPressed() {
  botAI.setForceStop(false);
  // botAI.gotoLoc(odom::TilePosition(0, 1));
  // botAI.turnTo(-90, 10);
  botAI.reverseDrive(24);
  botAI.setForceStop(true);
}

// Setup callbacks and initialize auton
void initializeSystems(void) {

  mainController.ButtonRight.pressed(rightPressed);

  // Imedietely stop the motors so then they don't spin when the robot boots up
  setMotors(0, 0, velocityUnits::pct);

  // Set brake modes
  leftMotorA.setBrake(vex::brakeType::coast);
  leftMotorB.setBrake(vex::brakeType::coast);
  leftMotorC.setBrake(vex::brakeType::coast);
  rightMotorA.setBrake(vex::brakeType::coast);
  rightMotorB.setBrake(vex::brakeType::coast);
  rightMotorC.setBrake(vex::brakeType::coast);

  catapultMotor.setBrake(vex::brakeType::coast);
  intakeMotor.setBrake(vex::brakeType::brake);

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


