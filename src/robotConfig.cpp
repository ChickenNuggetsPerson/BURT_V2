
#include "vex.h"
#include "tasks.h"

using namespace vex;

brain Brain;


// Drivetrain

motor leftMotorA = motor(PORT11, ratio36_1, false);
motor leftMotorB = motor(PORT12, ratio36_1, false);
motor_group LeftDriveSmart = motor_group(leftMotorA, leftMotorB);

motor rightMotorA = motor(PORT20, ratio36_1, true);
motor rightMotorB = motor(PORT19, ratio36_1, true);
motor_group RightDriveSmart = motor_group(rightMotorA, rightMotorB);

drivetrain Drivetrain = drivetrain(LeftDriveSmart, RightDriveSmart, 319.19, 254, 254, mm, 1);

controller mainController = controller(primary);


void initBot( void ) {
    task mainControllerTask(controllerTask);
}