
#include "robotConfig.h"
#include "controllerDisplayerAPI.h"
using namespace vex;


// Controller Stuff

double motorMaxSpeed = 0.8;
double boostMotorSpeed = 1;
void setNewDriveMax(double max) {
  motorMaxSpeed = max;
};


int motorFL = 0;
int motorFR = 0;
int motorBL = 0;
int motorBR = 0;

int leftFB = 0;
int rightFB = 0;

int turn = 0;


controlSystem::MotorController motorController(&mainController);

bool questioning = false;
void toggleHold() {
  if (questioning)
    return;
  frontArmHolder.setRunning(true);
  frontArmHolder.setNewVal(70);
}

bool armsOpen = false;
void toggleArms() {
  armsOpen = !armsOpen;
}
void setWingsOpen(bool status) {
  armsOpen = status;
};

bool reversedDrive = false;

int controllerTask() {

  vex::digital_out sideArms = vex::digital_out(Brain.ThreeWirePort.A);

  mainController.ButtonA.pressed(toggleHold);
  mainController.ButtonB.pressed(toggleArms);

  DEBUGLOG("Starting controller");

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

  frontArmMotor.spin(fwd);
  //frontArmMotor.setBrake(brakeType::brake);
  frontArmMotor.setVelocity(0, percent);
  frontArmMotor.setPosition(0, degrees);
  frontArmHolder.setRunning(false);

  cataArmMotor.spin(fwd);
  cataArmMotor.setVelocity(0, percent);
  cataArmMotor.setBrake(brakeType::coast);

  bool wasStopped = false;

  bool tankDrive = true;
  if (Brain.SDcard.isInserted()) {
    tankDrive = (misc::readFile(std::string(systemConfigFolder + systemDriveModePath).c_str()) == 1);
  }
  //reversedDrive = false;

  int frontArmVal = 0;
  int cataArmMove = 0;

  // Main driving loop
  while(true) {

    if(Competition.isDriverControl()) {

      // Main Loop for geting controller input
      if (!inertialSensor.isCalibrating()) {
        leftFB = mainController.Axis3.position();
        rightFB = mainController.Axis2.position();

        leftFB = leftFB * (mainController.ButtonL1.pressing() ? boostMotorSpeed : motorMaxSpeed);
        rightFB = rightFB * (mainController.ButtonL1.pressing() ? boostMotorSpeed : motorMaxSpeed);

        turn = mainController.Axis1.position() * (mainController.ButtonL1.pressing() ? boostMotorSpeed : motorMaxSpeed);
      }
      frontArmVal = 0;
      if (mainController.ButtonR2.pressing()) {
        frontArmVal = 8;
      }
      if (mainController.ButtonL2.pressing()) {
        frontArmVal = -8;
      }

      if ((mainController.ButtonR2.pressing() || mainController.ButtonL2.pressing()) && frontArmHolder.getRunning()) {
        frontArmHolder.setRunning(false);
      }

      cataArmMove = 0;
      if (mainController.ButtonR1.pressing()) {
        cataArmMove = 10;
      }
      
    }
    
    if (tankDrive) { // Andrew Drive
      if (false) {
        motorFL = rightFB * -1;
        motorFR = leftFB * -1;

        motorBL = rightFB * -1;
        motorBR = leftFB * -1;
      } else {
        motorFL = leftFB;
        motorFR = rightFB;

        motorBL = leftFB;
        motorBR = rightFB;
      }

    } else { // Hayden Drive
      if (true) { leftFB *= -1; }  // Reverse driving when arms are open
      motorFL = leftFB + turn;
      motorFR = leftFB - turn;

      motorBL = leftFB + turn;
      motorBR = leftFB - turn;   
      
    }


    if ( !botAI.running || botAI.getForceStop()) {

      if (wasStopped) { // Refresh the motors after auton
        leftMotorA.spin(fwd);
        leftMotorB.spin(fwd);
        rightMotorA.spin(fwd);
        rightMotorB.spin(fwd);

        leftMotorA.setVelocity(0, percent);
        leftMotorB.setVelocity(0, percent);
        rightMotorA.setVelocity(0, percent);
        rightMotorB.setVelocity(0, percent);

        wasStopped = false;
      }

      leftMotorA.setVelocity(motorFL, percentUnits::pct);
      leftMotorB.setVelocity(motorBL, percentUnits::pct);
      rightMotorA.setVelocity(motorFR, percentUnits::pct);
      rightMotorB.setVelocity(motorBR, percentUnits::pct);

      frontArmMotor.spin(fwd, frontArmVal, voltageUnits::volt);
      cataSystem.setSpeed(cataArmMove);

      //DEBUGLOG(armsOpen ? "true" : "false");
      sideArms.set(armsOpen);

    } else {
      wasStopped = true;
    }

    // iterate over holders
    frontArmHolder.iterate();

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

  mainController.Screen.print("X: ");
  mainController.Screen.print(currentPos.x);
  mainController.Screen.newLine();
  mainController.Screen.print("Y: ");
  mainController.Screen.print(currentPos.y);
  mainController.Screen.newLine();
  mainController.Screen.print("Rot: ");
  mainController.Screen.print(misc::limitAngle(misc::radToDegree(currentPos.rot)));            
  //mainController.Screen.print((int)cataArmMotor.position(rotationUnits::deg) % 360);


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
  questioning = true;
  while (rendering) { wait(1, timeUnits::msec); }
  int result = pickOption(options, &mainController);
  haltRender = false;
  questioning = false;
  return result;
};