
#include "vex.h"

#include "robotConstants.h"

#include "tasks.h"
#include "displayer.h"
#include "auton.h"
#include "odometry.h"
#include "pid.h"
#include "mainController.h"
#include "altController.h"
#include "misc.h"

using namespace vex;

void startTasks(void);

extern competition Competition;
extern brain Brain;
extern OdometrySystem Odometry;
extern ai botAI;

extern motor leftMotorA;
extern motor leftMotorB;
extern motor_group LeftDriveSmart;

extern motor rightMotorA;
extern motor rightMotorB;
extern motor_group RightDriveSmart;

extern smartdrive Drivetrain;

extern controller mainController;
extern controller altController;

extern inertial inertialSensor;

