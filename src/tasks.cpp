

// All of the tasks that run separately

#include "vex.h"
#include "tasks.h"

using namespace vex;


void pre_auton(void) {
    initBot();
}


void autonomous(void) {

}


void usercontrol(void) {
  wait(1, seconds);
}


// Controller Stuff


int motorFL = 0;
int motorFR = 0;
int motorBL = 0;
int motorBR = 0;

int strafeFBL = 0;
int strafeFBR = 0;
int strafeLRL = 0;
int strafeLRR = 0;

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
    if(!autonRunning) {

      if (!inertialSensor.isCalibrating()) {
        strafeFBL = mainController.Axis3.position();
        strafeFBR = mainController.Axis2.position();
      }

    }

    motorFL = strafeFBL;
    motorFR = strafeFBR;

    motorBL = strafeFBL;
    motorBR = strafeFBR;

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

