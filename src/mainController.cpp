
#include "robotConfig.h"
#include "controllerDisplayerAPI.h"
using namespace vex;


// Controller Stuff

int motorFL = 0;
int motorFR = 0;
int motorBL = 0;
int motorBR = 0;

int leftFB = 0;
int rightFB = 0;

int turn = 0;

int controllerTask() {

  std::cout << "Starting controller" << std::endl;

  // process the controller input every 20 milliseconds
  // update the motors based on the input values

  leftMotorA.spin(fwd);
  leftMotorB.spin(fwd);
  rightMotorA.spin(fwd);
  rightMotorB.spin(fwd);

  leftMotorA.setVelocity(0, percent);
  leftMotorB.setVelocity(0, percent);
  rightMotorA.setVelocity(0, percent);
  rightMotorB.setVelocity(0, percent);


  bool tankDrive = true;
  if (Brain.SDcard.isInserted()) {
    tankDrive = (misc::readFile(std::string(systemConfigFolder + systemDriveModePath).c_str()) == 1);
  }

  // Main driving loop
  while(true) {

    if(Competition.isDriverControl()) {

      if (!inertialSensor.isCalibrating()) {
        leftFB = mainController.Axis3.position();
        rightFB = mainController.Axis2.position();

        turn = mainController.Axis1.position();
      }

    }
    
    if (tankDrive) {
      motorFL = leftFB;
      motorFR = rightFB;

      motorBL = leftFB;
      motorBR = rightFB;
    } else {
      motorFL = leftFB + turn;
      motorFR = leftFB - turn;

      motorBL = leftFB + turn;
      motorBR = leftFB - turn;      
    }


    if ( !botAI.running ) {

      leftMotorA.spin(fwd);
      leftMotorB.spin(fwd);
      rightMotorA.spin(fwd);
      rightMotorB.spin(fwd);

      leftMotorA.setVelocity(motorFL, percent);
      leftMotorB.setVelocity(motorBL, percent);
      rightMotorA.setVelocity(motorFR, percent);
      rightMotorB.setVelocity(motorBR, percent);
      
    }
    
    // wait before repeating the process
    vex::wait(20, msec);
  }
  return 0;
}



ControllerMessage displayMessage;
bool rendering = false;
bool haltRender = false;

// The main controller rendering task
void mainControllerRender() {

  if (haltRender) { return; }
  rendering = true;

  mainController.Screen.clearScreen();
  mainController.Screen.setCursor(1, 1);

  odom::Position currentPos = Odometry.currentPos();

  if (displayMessage.endTime > Brain.timer(timeUnits::msec)) {
    mainController.Screen.print(displayMessage.text);
    return;
  }

  mainController.Screen.print(currentPos.x);
  mainController.Screen.newLine();
  mainController.Screen.print(currentPos.y);
  mainController.Screen.newLine();
  mainController.Screen.print(misc::limitAngle(misc::radToDegree(currentPos.rot)));            


  rendering = false;
}





void mainControllerMessage(const char* text, int timeout) {
  displayMessage.text = text;
  displayMessage.endTime = Brain.timer(timeUnits::msec) + (1000 * timeout);
  mainController.rumble("..");
};
bool mainControllerOverlay(const char* question, const char* trueOption, const char* falseOption) {
  if (!mainController.installed()) { return false; }
  haltRender = true;
  while (rendering) { wait(1, timeUnits::msec); }
  bool result = displayOverlay(ControllerOverlay(question, falseOption, trueOption), &mainController);
  haltRender = false;
  return result;
};
int mainControllerPickOption(std::vector <const char*> options) {
  if (!mainController.installed()) { return 0; }
  haltRender = true;
  while (rendering) { wait(1, timeUnits::msec); }
  int result = pickOption(options, &mainController);
  haltRender = false;
  return result;
};