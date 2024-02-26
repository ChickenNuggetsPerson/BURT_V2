
#include "vex.h"
#include "robotConfig.h"

using namespace vex;

competition Competition;
brain Brain;
debugSystem::Debugger WSDebugger;
auton::aiQueueSystem queuingSystem;
odom::OdometrySystem Odometry;
auton::AutonSystem botAI = auton::AutonSystem(&Odometry, &queuingSystem);

// Drivetrain

inertial inertialSensor = inertial(inertialPort);

rotation leftEncoder = rotation(leftEncoderPort, false);
rotation rightEncoder = rotation(rightEncoderPort, false);

motor leftMotorA = motor(leftMotorAPort, ratio18_1, !devRobot);
motor leftMotorB = motor(leftMotorBPort, ratio18_1, !devRobot);
motor leftMotorC = motor(leftMotorCPort, ratio18_1, !devRobot);

motor rightMotorA = motor(rightMotorAPort, ratio18_1, devRobot);
motor rightMotorB = motor(rightMotorBPort, ratio18_1, devRobot);
motor rightMotorC = motor(rightMotorCPort, ratio18_1, devRobot);

motor catapultMotor = motor(cataMotorPort, ratio36_1, true);
motor intakeMotor = motor(intakeMotorPort, ratio18_1, true);

controller mainController = controller(primary);
controller altController = controller(partner);


// Start all of the background tasks
void startTasks( void ) {

    task mainControllerTask(controllerTask);
    
    task brainDisplayerTask(brainDisplayer);
    task controllerDisplayerTask(controllerDisplay);

    brainFancyDebug("Tasks Started", vex::color::yellow);

}