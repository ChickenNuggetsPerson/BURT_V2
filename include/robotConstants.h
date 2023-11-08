#pragma once

#include "pid.h"
#include "odometry.h"


static bool devRobot = false;


// Define C++ Macros

// A debug macro that takes in any inputs and uses std::cout with them
#define DEBUGLOG(...) do { \
    variablePrint(__VA_ARGS__); \
    std::cout << std::endl; \
} while(0);

template<typename T>
void variablePrint(const T& arg) {
    std::cout << arg;
}

template<typename T, typename... Args>
void variablePrint(const T& arg, const Args&... args) {
    std::cout << arg;
    variablePrint(args...);
}



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

static double PI = M_PI; // PI variable used for all methods


static double motorWarnTemp = 50; // The temp in c


// Define Ports
static const int32_t leftMotorAPort = vex::PORT6;
static const int32_t leftMotorBPort = vex::PORT3;
static const int32_t rightMotorAPort = vex::PORT9;
static const int32_t rightMotorBPort = vex::PORT2;

static const int32_t inertialPort = vex::PORT16;

static const int32_t leftEncoderPort = vex::PORT18;
static const int32_t rightEncoderPort = vex::PORT17;

static const int32_t frontArmMotorPort = vex::PORT10;
static const int32_t cataMotorPort = vex::PORT5;


// Odom Constants
static odom::TilePosition ODOM_DEFAULT_RESET_POS(0, 0, 0);

// Auton Constants For Test Bot
/*
static PIDConfig AUTON_TURNTO_PID_CONFIG(0.15, 0, 0.25);

static PIDConfig AUTON_GOTO_DRIVE_PID_CONFIG(0.2, 0.00, 0.06);
static PIDConfig AUTON_GOTO_TURN_VEl_PID_CONFIG(0.08, 0.00, 0.00);
*/

// Auton Constants For Main Bot

static pid::PIDConfig AUTON_TURNTO_PID_CONFIG(0.072, 0.0, 0.0); 

static pid::PIDConfig AUTON_GOTO_DRIVE_PID_CONFIG(0.5, 0.0, 0.0);
static pid::PIDConfig AUTON_GOTO_TURN_VEl_PID_CONFIG(0.12, 0.0, 0.0);
static pid::PIDConfig AUTON_GOTO_TURN_VOLT_PID_CONFIG(0.05, 0.0, 0.0);


// Auton Start Positions
static odom::TilePosition AUTON_START_LEFT(1,  -0.1291, 0);
static odom::TilePosition AUTON_START_RIGHT(4,  -0.1291, 0);
static odom::TilePosition AUTON_START_SKILLS(1, -0.1291, 0);
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