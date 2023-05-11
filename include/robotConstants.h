#include "pid.h"
#include "odometry.h"
#pragma once


// Define Robot Measurements 

static double wheelDiameter = 4;

static double encoderDist = 4.5;
static double backEncoderDist = 4.5; // use if we decide to use back encoders


// Misc Constants

static double PI = 3.14159; // PI variable used for all methods
static int tileWidth = 24; // Vex Tiles are 2ft across

static int motorWarnTemp = 110; // The temp to start warning the user about motor temperatures


// Define Ports
static const int32_t leftMotorAPort = vex::PORT1;
static const int32_t leftMotorBPort = vex::PORT10;
static const int32_t rightMotorAPort = vex::PORT2;
static const int32_t rightMotorBPort = vex::PORT8;

static const int32_t inertialPort = vex::PORT3;




// Odom Constants
static TilePosition ODOM_DEFAULT_RESET_POS(0, 0, 0);

// Auton Constants
static PIDConfig AUTON_TURNTO_PID_CONFIG(0.10, 0, 0.25);

static PIDConfig AUTON_GOTO_DRIVE_PID_CONFIG(0.2, 0.00, 0.00);
static PIDConfig AUTON_GOTO_TURN_PID_CONFIG(0.05, 0.00, 0.00);

static Position AUTON_START_POS1(0, 0, 0); // Configure these when the game releases
static Position AUTON_START_POS2(0, 0, 0);




// Config File Paths
static std::string systemConfigFolder = "systemConfig/";
static std::string systemArchivePath = "archiveLogs.txt";


// Config Log Paths
static std::string systemLogFolder = "Logs/";
static std::string systemArchiveFolder = "logStorage/";
static std::string systemCompLogsFolder = "compLogs/";



