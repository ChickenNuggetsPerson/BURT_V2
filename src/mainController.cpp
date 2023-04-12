
#include "robotConfig.h"
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



  while(true) {

    if(Competition.isDriverControl()) {

      if (!inertialSensor.isCalibrating()) {
        leftFB = mainController.Axis3.position();
        rightFB = mainController.Axis3.position();

        turn = mainController.Axis1.position();
      }

    }
    

    motorFL = leftFB + turn;
    motorFR = rightFB - turn;

    motorBL = leftFB + turn;
    motorBR = rightFB - turn;

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

