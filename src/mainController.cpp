
#include "robotConfig.h"
using namespace vex;



motor Test1(PORT1, vex::gearSetting::ratio18_1);
motor Test2(PORT2);

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

  Test1.spin(fwd);
  Test2.spin(fwd);

  while(true) {

    if(Competition.isDriverControl()) {

      if (!inertialSensor.isCalibrating()) {
        leftFB = mainController.Axis3.position();
        rightFB = mainController.Axis2.position();
      }

    }

    Test1.setVelocity(mainController.Axis3.position(), percent);
    Test2.setVelocity(mainController.Axis3.position(), percent);
    

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
    vex::wait(20, msec);
  }
  return 0;
}

