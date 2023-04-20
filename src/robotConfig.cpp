
#include "vex.h"
#include "robotConfig.h"

using namespace vex;

competition Competition;
brain Brain;
OdometrySystem Odometry;
ai botAI = ai(&Odometry);

// Drivetrain

inertial inertialSensor = inertial(PORT3);

motor leftMotorA = motor(PORT1, ratio18_1, false);
motor leftMotorB = motor(PORT10, ratio18_1, false);
motor_group LeftDriveSmart = motor_group(leftMotorA, leftMotorB);

motor rightMotorA = motor(PORT2, ratio18_1, true);
motor rightMotorB = motor(PORT8, ratio18_1, true);
motor_group RightDriveSmart = motor_group(rightMotorA, rightMotorB);

smartdrive Drivetrain = smartdrive(LeftDriveSmart, RightDriveSmart, inertialSensor, 319.19, 254, 254, mm, 1);

controller mainController = controller(primary);
controller altController = controller(partner);




void startTasks( void ) {

    task mainControllerTask(controllerTask);
    
    task brainDisplayerTask(brainDisplayer);
    task controllerDisplayerTask(controllerDisplay);

    brainFancyDebug("Tasks Started", vex::color::yellow);

}