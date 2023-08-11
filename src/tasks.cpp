

// All of the tasks that run separately

#include "robotConfig.h"
#include "tasks.h"

using namespace vex;

// Debug Functions
void rightPressed() {
  //queuingSystem.addToQueue(auton::autonMovement(AUTON_MOVE_GOTO, botAI.getStartPos()));
  queuingSystem.clear();
};
void leftPressed() {
  queuingSystem.addToQueue(auton::autonMovement(AUTON_MOVE_GOTO, botAI.getStartPos()));
  queuingSystem.runQueue();
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