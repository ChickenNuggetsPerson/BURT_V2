

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


int controllerTask() {

    leftMotorA.spin(fwd);
    leftMotorB.spin(fwd);
    rightMotorA.spin(fwd);
    rightMotorB.spin(fwd);

    leftMotorA.setVelocity(0, percent);
    leftMotorB.setVelocity(0, percent);
    rightMotorA.setVelocity(0, percent);
    rightMotorB.setVelocity(0, percent);

    while(true) {

        

        wait(1, seconds);
    } 

    return 0;
}

