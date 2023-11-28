
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
rotation rightEncoder = rotation(rightEncoderPort, true);

motor leftMotorA = motor(leftMotorAPort, ratio18_1, !devRobot);
motor leftMotorB = motor(leftMotorBPort, ratio18_1, !devRobot);
motor_group LeftDriveSmart = motor_group(leftMotorA, leftMotorB);

motor rightMotorA = motor(rightMotorAPort, ratio18_1, devRobot);
motor rightMotorB = motor(rightMotorBPort, ratio18_1, devRobot);
motor_group RightDriveSmart = motor_group(rightMotorA, rightMotorB);

smartdrive Drivetrain = smartdrive(LeftDriveSmart, RightDriveSmart, inertialSensor, 319.19, 254, 254, mm, 1);

motor frontArmMotor(frontArmMotorPort, ratio36_1, true);
controlSystem::MotorHolder frontArmHolder(&frontArmMotor, pid::PIDConfig(0.2, 0.00, 0.0), rotationUnits::deg);

motor cataArmMotor(cataMotorPort, gearSetting::ratio36_1, true);
controlSystem::CatapultSystem cataSystem(&cataArmMotor);

controller mainController = controller(primary);
controller altController = controller(partner);



// Start all of the background tasks
void startTasks( void ) {

    task mainControllerTask(controllerTask);
    
    task brainDisplayerTask(brainDisplayer);
    task controllerDisplayerTask(controllerDisplay);

    brainFancyDebug("Tasks Started", vex::color::yellow);

}