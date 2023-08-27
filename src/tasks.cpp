

// All of the tasks that run separately

#include "robotConfig.h"
#include "tasks.h"

using namespace vex;

// Debug Functions
void rightPressed() {
  //queuingSystem.clear();
  //botAI.gotoLoc(botAI.getStartPos());
  frontArmHolder.setRunning(true);
  queuingSystem.clear();
  queuingSystem.addToQueue(auton::autonMovement(AUTON_MOVE_PICKUP_ACORN));
  queuingSystem.runQueue();
  frontArmHolder.setRunning(false);

};
void leftPressed() {
  //queuingSystem.runQueue();
  frontArmHolder.setNewVal(0);
  frontArmHolder.setRunning(true);
  wait(1, seconds);
  frontArmHolder.setRunning(false);
};
void upPressed() {

}
void downPressed() {
  
}

// Setup callbacks and initialize auton
void pre_auton(void) {

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


void autonomous(void) { queuingSystem.autonStarted(); }


void usercontrol(void) {
  brainFancyDebug("UserControl Started", vex::color::green, true);
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