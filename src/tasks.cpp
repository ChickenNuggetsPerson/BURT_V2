

// All of the tasks that run separately

#include "vex.h"
#include "tasks.h"

using namespace vex;


void debugDisable() {Competition.test_disable();};
void debugDriver() {Competition.test_driver();};
void debugAuton() {Competition.test_auton();};

void pre_auton(void) {

  altController.ButtonDown.pressed(debugDisable);
  altController.ButtonLeft.pressed(debugAuton);
  altController.ButtonRight.pressed(debugDriver);

  botAI.init();
  startTasks();
}


void autonomous(void) {
  brainDebug("Auton Started");
}


void usercontrol(void) {
  brainDebug("UserControl Started");
}


void whenStarted(void){
  controllerDisplay();
}




// Controller Stuff

int motorFL = 0;
int motorFR = 0;
int motorBL = 0;
int motorBR = 0;

int leftFB = 0;
int rightFB = 0;

int controllerTask() {

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


  while(true) {

    if(Competition.isDriverControl()) {

      if (!inertialSensor.isCalibrating()) {
        leftFB = mainController.Axis3.position();
        rightFB = mainController.Axis2.position();
      }

    }

    motorFL = leftFB;
    motorFR = rightFB;

    motorBL = leftFB;
    motorBR = rightFB;

    if ( !Drivetrain.isMoving() ) {

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
    wait(20, msec);
  }
  return 0;
}

