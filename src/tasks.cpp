

// All of the tasks that run separately

#include "robotConfig.h"
#include "tasks.h"

using namespace vex;

// Debug Functions
void rightPressed() {
  botAI.setForceStop(false);
  //botAI.driveDist(24*2); 
  botAI.driveDist(24);
  //botAI.gotoLoc(botAI.getStartPos());
  botAI.setForceStop(true);
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

  WSDebugger.init();
  if (!WSDebugger.isSending()) {return;}
  while (true) {
      WSDebugger.sendData("RMA", rightMotorA.temperature(vex::percent));
      WSDebugger.sendData("RMB", rightMotorB.temperature(vex::percent));
      WSDebugger.sendData("LMA", leftMotorA.temperature(vex::percent));
      WSDebugger.sendData("LMB", leftMotorB.temperature(vex::percent));
      WSDebugger.sendData("bat", Brain.Battery.capacity());
      WSDebugger.sendData("watt", Brain.Battery.current(currentUnits::amp) * Brain.Battery.voltage(voltageUnits::volt));

      wait(5, vex::timeUnits::sec);
  }
}