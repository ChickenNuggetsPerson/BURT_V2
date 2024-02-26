
#include "robotConfig.h"
#include "controllerDisplayerAPI.h"
using namespace vex;


// Catapult Code
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


// Wing Code
bool leftWing = false;
bool rightWing = false;
void toggleWings() {
  leftWing = !leftWing;
  rightWing = leftWing;
}
void toggleLeft() {
  leftWing = !leftWing;
}
void toggleRight() {
  rightWing = !rightWing;
}
void setWings(WingStates state) {
  switch (state) {
    case WingStates::W_Close:
      leftWing = false;
      rightWing = false;
      break;

    case WingStates::W_All_Open:
      leftWing = true;
      rightWing = true;
      break;

    case WingStates::W_L_Open:
      leftWing = true;
      rightWing = false;
      break;

    case WingStates::W_R_Open:
      leftWing = false;
      rightWing = true;
      break;
  }
}
auto L1Btn = ButtonSystem(&Brain, &mainController, ButtonSystem_Btn::L1, false, toggleWings, nullptr, 10);
auto YBtn = ButtonSystem(&Brain, &mainController, ButtonSystem_Btn::Y, false, toggleLeft, nullptr, 10);
auto ABtn = ButtonSystem(&Brain, &mainController, ButtonSystem_Btn::A, false, toggleRight, nullptr, 10);
void wingCheck() {
  L1Btn.check();
  YBtn.check();
  ABtn.check();
}


// Intake Code
int intakeDir = 0;
void intakeForward() {
  intakeDir = 1;
}
void intakeOutward() {
  intakeDir = -1;
}
void stopIntake() {
  intakeDir = 0;
}
void setIntakeDir(int dir) {
  intakeDir = dir;
}
auto R1Btn = ButtonSystem(&Brain, &mainController, ButtonSystem_Btn::R1, false, intakeForward, stopIntake, 100);
auto R2Btn = ButtonSystem(&Brain, &mainController, ButtonSystem_Btn::R2, false, intakeOutward, stopIntake, 100);
void intakeCheck() {
  R1Btn.check();
  R2Btn.check();
  intakeMotor.spin(vex::directionType::fwd, 10 * intakeDir, vex::voltageUnits::volt);
}

void setMotors(double leftAmt, double rightAmt, vex::velocityUnits units) {
  leftMotorA.spin(fwd);
  leftMotorB.spin(fwd);
  leftMotorC.spin(fwd);

  rightMotorA.spin(fwd);
  rightMotorB.spin(fwd);
  rightMotorC.spin(fwd);
  
  leftMotorA.setVelocity(leftAmt, units);
  leftMotorB.setVelocity(leftAmt, units);
  leftMotorC.setVelocity(leftAmt, units);

  rightMotorA.setVelocity(rightAmt, units);
  rightMotorB.setVelocity(rightAmt, units);
  rightMotorC.setVelocity(rightAmt, units);
}
void setMotors(double leftAmt, double rightAmt, vex::voltageUnits units) {
  leftMotorA.spin(fwd, leftAmt, units);
  leftMotorB.spin(fwd, leftAmt, units);
  leftMotorC.spin(fwd, leftAmt, units);

  rightMotorA.spin(fwd, rightAmt, units);
  rightMotorB.spin(fwd, rightAmt, units);
  rightMotorC.spin(fwd, rightAmt, units);
}
inline double lerp(double start, double end, double amt) { return start + ((end - start) * amt); };

// DriveSystem driveSystem = DriveSystem(&Brain, 8, 15);
bool questioning = false;
int controllerTask() {

  vex::digital_out leftWingSolenoid = vex::digital_out(Brain.ThreeWirePort.A);
  vex::digital_out rightWingSolenoid = vex::digital_out(Brain.ThreeWirePort.B);

  // Set up variables
  double leftMotors = 0;
  double rightMotors = 0;

  double leftFB = 0;
  double rightFB = 0;

  double turn = 0;

  DEBUGLOG("Starting controller");

  // process the controller input every 20 milliseconds
  // update the motors based on the input values

  setMotors(0, 0, velocityUnits::pct);


  bool wasStopped = false;

  bool tankDrive = true;
  // if (Brain.SDcard.isInserted()) {
    // tankDrive = (misc::readFile(std::string(systemConfigFolder + systemDriveModePath).c_str()) == 0);
  // }

  int frontArmVal = 0;
  int cataArmMove = 0;

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
      leftMotors = leftFB;
      rightMotors = rightFB;

    } else { // Hayden Drive

      leftMotors = leftFB + turn;
      rightMotors = leftFB - turn;   
    }


    if ( !botAI.running || botAI.getForceStop()) {

      if (wasStopped) { // Refresh the motors after auton

        setMotors(0, 0, velocityUnits::pct);

        wasStopped = false;
      }

      double avgTemp = (leftMotorA.temperature(temperatureUnits::celsius) + leftMotorA.temperature(temperatureUnits::celsius)) / 2;
      double maxVolt = 10;
      if (avgTemp > 55) {
        maxVolt = 14;
      }
      setMotors(
        lerp(0, maxVolt, leftMotors / 100.0), 
        lerp(0, maxVolt, rightMotors / 100.0), 
        vex::voltageUnits::volt
      );

    } else {
      wasStopped = true;
    }

    // iterate over systems
    wingCheck();
    leftWingSolenoid.set(leftWing);
    rightWingSolenoid.set(rightWing);
    intakeCheck();
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
    mainController.Screen.print(displayMessage.text.c_str());
    return;
  }

  mainController.Screen.print("X: ");
  mainController.Screen.print(currentPos.x);

  mainController.Screen.print("   ");
  mainController.Screen.print(floor(Brain.timer(vex::timeUnits::sec)));

  mainController.Screen.newLine();
  mainController.Screen.print("Y: ");
  mainController.Screen.print(currentPos.y);
  mainController.Screen.newLine();
  mainController.Screen.print("Rot: ");
  mainController.Screen.print(misc::limitAngle(misc::radToDegree(currentPos.rot)));            

  mainController.Screen.setCursor(3, 15);
  mainController.Screen.print(Odometry.getVelocity());


  rendering = false;
}





void mainControllerMessage(std::string text, int timeout) {
  displayMessage.text = text;
  displayMessage.endTime = Brain.timer(timeUnits::msec) + (1000 * timeout);
  mainController.rumble(".");
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