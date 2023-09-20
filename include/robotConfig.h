#pragma once

#include "vex.h"

#include "robotConstants.h"

#include "tasks.h"
#include "displayer.h"
#include "auton.h"
#include "odometry.h"
#include "paths.h"
#include "pid.h"
#include "mainController.h"
#include "altController.h"
#include "controlSystem.h"
#include "misc.h"


using namespace vex;

void startTasks(void);

extern competition Competition;
extern brain Brain;
extern auton::aiQueueSystem queuingSystem;
extern odom::OdometrySystem Odometry;
extern auton::AutonSystem botAI;

extern motor leftMotorA;
extern motor leftMotorB;
extern motor_group LeftDriveSmart;

extern motor rightMotorA;
extern motor rightMotorB;
extern motor_group RightDriveSmart;

extern smartdrive Drivetrain;

extern motor frontArmMotor;
extern controlSystem::MotorHolder frontArmHolder;

extern motor cataArmMotor;

extern controller mainController;
extern controller altController;

extern inertial inertialSensor;
extern rotation leftEncoder;
extern rotation rightEncoder;



#include "debuger.h"
extern debugSystem::Debugger WSDebugger;
