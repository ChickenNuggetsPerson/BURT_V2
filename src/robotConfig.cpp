
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

rotation leftEncoder = rotation(leftEncoderPort, true);
rotation rightEncoder = rotation(rightEncoderPort, true);

motor leftMotorA = motor(leftMotorAPort, ratio18_1, true);
motor leftMotorB = motor(leftMotorBPort, ratio18_1, true);
motor leftMotorC = motor(leftMotorCPort, ratio18_1, false);

motor rightMotorA = motor(rightMotorAPort, ratio18_1, false);
motor rightMotorB = motor(rightMotorBPort, ratio18_1, false);
motor rightMotorC = motor(rightMotorCPort, ratio18_1, true);

motor catapultMotor = motor(cataMotorPort, ratio36_1, false);
motor intakeMotor = motor(intakeMotorPort, ratio18_1, true);

controller mainController = controller(primary);
controller altController = controller(partner);


// Start all of the background tasks
void startTasks( void ) {

    task mainControllerTask(controllerTask, vex::task::taskPriorityHigh);
    
    task brainDisplayerTask(brainDisplayer, vex::task::taskPrioritylow);
    task controllerDisplayerTask(controllerDisplay, vex::task::taskPriorityNormal);

    // brainFancyDebug("Tasks Started", vex::color::yellow);

}