

// All of the tasks that run separately

#include "robotConfig.h"
#include "tasks.h"

using namespace vex;

// Debug Functions
void rightPressed() {

  for(int i = 0; i < 5; i++) {
    cataSystem.reset();
    cataSystem.launch();
  }


  return;
  queuingSystem.clear();
  
  std::vector<const char*> options;

  options.push_back("Pickup Acorn");
  options.push_back("Drop Acorn");
  options.push_back("Set Arm");
  options.push_back("Release Arm");

  switch (mainControllerPickOption(options)) {
    case 0:
      queuingSystem.addToQueue(auton::autonMovement(AUTON_MOVE_PICKUP_ACORN));
      break;
    case 1:
      queuingSystem.addToQueue(auton::autonMovement(AUTON_MOVE_DROPOFF_ACORN));
      break;
    case 2:
      queuingSystem.addToQueue(auton::autonMovement(AUTON_MOVE_ARM_SET, 75));
      break;
    case 3:
      queuingSystem.addToQueue(auton::autonMovement(AUTON_MOVE_ARM_RELEASE));
      break;
  }

  queuingSystem.runQueue();
};
void leftPressed() {
  queuingSystem.clear();
  queuingSystem.addToQueue(auton::autonMovement(AUTON_MOVE_GOTO, botAI.getStartPos()));
  queuingSystem.runQueue();
};
void upPressed() {

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