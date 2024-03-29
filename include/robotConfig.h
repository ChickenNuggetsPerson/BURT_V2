#pragma once

#include "vex.h"

#include "robotConstants.h"

#include "misc.h"
#include "stateMachine.h"
#include "tasks.h"
#include "displayer.h"
#include "auton.h"
#include "odometry.h"
#include "paths.h"
#include "pid.h"
#include "mainController.h"
#include "altController.h"
#include "controlSystem.h"



using namespace vex;

void startTasks(void);

extern competition Competition;
extern brain Brain;
extern auton::aiQueueSystem queuingSystem;
extern odom::OdometrySystem Odometry;
extern auton::AutonSystem botAI;

extern motor leftMotorA;
extern motor leftMotorB;
extern motor leftMotorC;

extern motor rightMotorA;
extern motor rightMotorB;
extern motor rightMotorC;

extern motor catapultMotor;
extern motor intakeMotor;

extern controller mainController;
extern controller altController;

extern inertial inertialSensor;
extern rotation leftEncoder;
extern rotation rightEncoder;



#include "debuger.h"
extern debugSystem::Debugger WSDebugger;
