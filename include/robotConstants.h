#pragma once

#include "pid.h"
#include "odometry.h"


static bool devRobot = false;

// Define Robot Measurements 

// Main Robot Consts
//  wheelDiameter: 2.5
//  encoderDist: 5.25
//  ratio: 5/3

// Dev Robot Const
//  wheelDiameter: 4
//  encoderDist: 4.5
//  ratio 1/1

// Set the ratio to 1/1 if using shaft encoders


static double wheelDiameter = 2.75;
static double motorRatio = 1;
static double wheelRatio = 1;

static double encoderDist = 4;
static double backEncoderDist = 4.5; // use if we decide to use back encoders


// Misc Constants

static double PI = 3.14159; // PI variable used for all methods


static double motorWarnTemp = 80.00; // The temp percent to start warning the user about motor temperatures


// Define Ports
static const int32_t leftMotorAPort = vex::PORT20;
static const int32_t leftMotorBPort = vex::PORT16;
static const int32_t rightMotorAPort = vex::PORT8;
static const int32_t rightMotorBPort = vex::PORT12;

static const int32_t inertialPort = vex::PORT9;

static const int32_t leftEncoderPort = vex::PORT6;
static const int32_t rightEncoderPort = vex::PORT7;


// Odom Constants
static odom::TilePosition ODOM_DEFAULT_RESET_POS(0, 0, 0);

// Auton Constants For Test Bot
/*
static PIDConfig AUTON_TURNTO_PID_CONFIG(0.15, 0, 0.25);

static PIDConfig AUTON_GOTO_DRIVE_PID_CONFIG(0.2, 0.00, 0.06);
static PIDConfig AUTON_GOTO_TURN_PID_CONFIG(0.08, 0.00, 0.00);
*/

// Auton Constants For Main Bot

static pid::PIDConfig AUTON_TURNTO_PID_CONFIG(0.14, 0, 0.1); 

static pid::PIDConfig AUTON_GOTO_DRIVE_PID_CONFIG(0.45, 0.00, 0.1);
static pid::PIDConfig AUTON_GOTO_TURN_PID_CONFIG(0.2, 0.0, 0.01);


// Auton Start Positions
static odom::TilePosition AUTON_START_LEFT(1, 0, 0);
static odom::TilePosition AUTON_START_RIGHT(4, 0, 0);
static odom::TilePosition AUTON_START_SKILLS(1, 0, 0);
// Auton JSON Path Locations
static std::string AUTON_PATH_FOLDER = "paths/";
static std::string AUTON_PATH_RIGHT_JSON = "right.json";
static std::string AUTON_PATH_LEFT_JSON = "left.json";
static std::string AUTON_PATH_SKILLS_JSON = "skills.json";



// Config File Paths
static std::string systemConfigFolder = "systemConfig/";
static std::string systemArchivePath = "archiveLogs.txt";
static std::string systemDriveModePath = "driveMode.txt";
static std::string systemEnableWSDebug = "enableWSDebug.txt";


// Config Log Paths
static std::string systemLogFolder = "Logs/";
static std::string systemArchiveFolder = "logStorage/";
static std::string systemCompLogsFolder = "compLogs/";





// Other Constants
static int TEAM_RED = 1;
static int TEAM_BLUE = 2;
static int TEAM_NULL = 0;