
#include "robotConfig.h"
#include "controllerDisplayerAPI.h"
using namespace vex;


//  Wing State Machine Functions
const double Wing_rotationOne = 30;
const double Wing_rotationTwo = 445;
const double Wing_rotationThree = 870;
const double Wing_rotationClose = 1000;
const double Wing_Speed = 2000;

#define WingBtnCheck() { int btn = getBtnPressed(); if (btn != -1) { return btn; } };
int getBtnPressed() {
  if (mainController.ButtonB.pressing()) // B
      return W_pos1;
  if (mainController.ButtonA.pressing()) // A
      return W_pos2;
  if (mainController.ButtonY.pressing()) // Y
      return W_pos3;
  if (mainController.ButtonX.pressing()) // X
      return W_close;
  if (mainController.ButtonDown.pressing()) // Down
      return W_CataAlign;

  if (mainController.ButtonR1.pressing() || mainController.ButtonR2.pressing() || mainController.ButtonL1.pressing() || mainController.ButtonL2.pressing())
      return W_loose;

  return -1;
}
int wingLooseState() {

  int leftDir = 0;
  int rightDir = 0;

  if (mainController.ButtonR1.pressing()) 
      rightDir++;
  if (mainController.ButtonR2.pressing()) 
      rightDir--;

  if (mainController.ButtonL1.pressing()) 
      leftDir++;
  if (mainController.ButtonL2.pressing()) 
      leftDir--;

  leftArmMotor.spin(vex::directionType::fwd, 10 * leftDir, vex::voltageUnits::volt);
  rightArmMotor.spin(vex::directionType::fwd, 10 * rightDir, vex::voltageUnits::volt);

  WingBtnCheck()

  return W_loose;
}
int wingPos1State() {
  rightArmMotor.spinToPosition(Wing_rotationOne, vex::rotationUnits::deg, Wing_Speed, vex::velocityUnits::rpm, false);
  leftArmMotor.spinToPosition(Wing_rotationOne, vex::rotationUnits::deg, Wing_Speed, vex::velocityUnits::rpm, false);

  WingBtnCheck()

  return W_pos1;
}
int wingPos2State() {
  rightArmMotor.spinToPosition(Wing_rotationTwo, vex::rotationUnits::deg, Wing_Speed, vex::velocityUnits::rpm, false);
  leftArmMotor.spinToPosition(Wing_rotationTwo, vex::rotationUnits::deg, Wing_Speed, vex::velocityUnits::rpm, false);

  WingBtnCheck()

  return W_pos2;
}
int wingPos3State() {
  rightArmMotor.spinToPosition(Wing_rotationThree, vex::rotationUnits::deg, Wing_Speed, vex::velocityUnits::rpm, false);
  leftArmMotor.spinToPosition(Wing_rotationThree, vex::rotationUnits::deg, Wing_Speed, vex::velocityUnits::rpm, false);

  WingBtnCheck()

  return W_pos3;
}
int wingCloseState() {
  rightArmMotor.spinToPosition(Wing_rotationClose, vex::rotationUnits::deg, Wing_Speed, vex::velocityUnits::rpm, false);
  leftArmMotor.spinToPosition(Wing_rotationClose, vex::rotationUnits::deg, Wing_Speed, vex::velocityUnits::rpm, false);

  WingBtnCheck()

  return W_close;
}
int wingCataAlignState() {
  // Fix this

  rightArmMotor.spinToPosition(Wing_rotationTwo, vex::rotationUnits::deg, Wing_Speed, vex::velocityUnits::rpm, false);
  leftArmMotor.spinToPosition(Wing_rotationTwo, vex::rotationUnits::deg, Wing_Speed, vex::velocityUnits::rpm, false);

  WingBtnCheck()

  return W_CataAlign;
}
int wingLeftAutonState() {

  rightArmMotor.spinToPosition(Wing_rotationOne, vex::rotationUnits::deg, Wing_Speed, vex::velocityUnits::rpm, false);
  leftArmMotor.spinToPosition(750, vex::rotationUnits::deg, Wing_Speed, vex::velocityUnits::rpm, false);

  WingBtnCheck()

  return W_LeftAutonPoleTouch;
}


bool cata_autoLaunch = false;
void startCatapult() {
    cata_autoLaunch = true;
}
void stopCatapult() {
    cata_autoLaunch = false;
}
void toggleCata() {
    cata_autoLaunch = !cata_autoLaunch;
}
auto upBtn = ButtonSystem(&Brain, &mainController, ButtonSystem_Btn::Up, false, startCatapult, stopCatapult, 0.00);
auto leftBtn = ButtonSystem(&Brain, &mainController, ButtonSystem_Btn::Left, true, toggleCata, toggleCata, 10);
void catapultCheck() {
    upBtn.check();
    leftBtn.check();

    if (cata_autoLaunch) {
        catapultMotor.spin(vex::directionType::fwd, 12, vex::voltageUnits::volt);
    } else {
        catapultMotor.spin(vex::directionType::fwd, 0, vex::voltageUnits::volt);
    }
}







bool questioning = false;
int controllerTask() {

  wingStateMachine.addState(W_loose, wingLooseState);
  wingStateMachine.addState(W_pos1, wingPos1State);
  wingStateMachine.addState(W_pos2, wingPos2State);
  wingStateMachine.addState(W_pos3, wingPos3State);
  wingStateMachine.addState(W_close, wingCloseState);
  wingStateMachine.addState(W_CataAlign, wingCataAlignState);
  wingStateMachine.addState(W_LeftAutonPoleTouch, wingLeftAutonState);
  wingStateMachine.start(W_pos1);


  // Set up variables
  double motorFL = 0;
  double motorFR = 0;
  double motorBL = 0;
  double motorBR = 0;

  double leftFB = 0;
  double rightFB = 0;

  double turn = 0;

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


  bool wasStopped = false;

  bool tankDrive = true;
  // if (Brain.SDcard.isInserted()) {
    // tankDrive = (misc::readFile(std::string(systemConfigFolder + systemDriveModePath).c_str()) == 0);
  // }
  //reversedDrive = false;

  int frontArmVal = 0;
  int cataArmMove = 0;


  // Set up the wings
  leftArmMotor.spin(vex::directionType::rev, 10, vex::voltageUnits::volt);
  rightArmMotor.spin(vex::directionType::rev, 10, vex::voltageUnits::volt);

	vex::wait(800, vex::timeUnits::msec);

	leftArmMotor.spin(vex::directionType::fwd, 0, vex::voltageUnits::volt);
  rightArmMotor.spin(vex::directionType::fwd, 0, vex::voltageUnits::volt);

	leftArmMotor.setPosition(0, vex::rotationUnits::deg);
  rightArmMotor.setPosition(0, vex::rotationUnits::deg);


  // Main driving loop
  while(true) {

    frontArmVal = 0;

    if (Competition.isDriverControl()) {

      // Main Loop for geting controller input
      if (!inertialSensor.isCalibrating()) {
        leftFB = mainController.Axis3.position();
        rightFB = mainController.Axis2.position();
        turn = mainController.Axis1.position();
      }

      cataArmMove = 0;
      if (mainController.ButtonR1.pressing()) {
        cataArmMove = 10;
      }
      
    }
    
    if (tankDrive) { // Andrew Drive
      motorFL = leftFB;
      motorFR = rightFB;

      motorBL = leftFB;
      motorBR = rightFB;

    } else { // Hayden Drive

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

    } else {
      wasStopped = true;
    }

    // iterate over systems
    wingStateMachine.iterate();
    catapultCheck();


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

  odom::TilePosition currentPos = Odometry.currentTilePos();

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