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

// #define DEBUGLOG(...) { };

template<typename T>
void variablePrint(const T& arg) {
    std::cout << arg;
}

template<typename T, typename... Args>
void variablePrint(const T& arg, const Args&... args) {
    std::cout << arg;
    variablePrint(args...);
}



// Set the ratio to 1/1 if using shaft encoders

static double wheelDiameter = 2.75;
static double motorGearTeeth = 2;
static double wheelGearTeeth = 1;

static double encoderDist = 4;
static double backEncoderDist = 4.5; // use if we decide to use back encoders


// Misc Constants

static double PI = M_PI; // PI variable used for all methods


static double motorWarnTemp = 60; // The temp in c


// Define Ports
static const int32_t leftMotorAPort = vex::PORT6;
static const int32_t leftMotorBPort = vex::PORT5;
static const int32_t leftMotorCPort = vex::PORT1;

static const int32_t rightMotorAPort = vex::PORT8;
static const int32_t rightMotorBPort = vex::PORT9;
static const int32_t rightMotorCPort = vex::PORT10;

static const int32_t inertialPort = vex::PORT18;

static const int32_t leftEncoderPort = vex::PORT17;
static const int32_t rightEncoderPort = vex::PORT7;

static const int32_t cataMotorPort = vex::PORT2;
static const int32_t intakeMotorPort = vex::PORT3; // Change this

// Odom Constants
static odom::TilePosition ODOM_DEFAULT_RESET_POS(0, 0, 0);

// Auton Start Positions
static odom::TilePosition AUTON_START_LEFT(1,   0, 0);
static odom::TilePosition AUTON_START_RIGHT(4,  0, 0);
static odom::TilePosition AUTON_START_SKILLS(1, 0, 0);
// Auton JSON Path Locations
static std::string AUTON_PATH_FOLDER = "paths/";
static std::string AUTON_PATH_RIGHT_JSON = "right.json";
static std::string AUTON_PATH_LEFT_JSON = "left.json";
static std::string AUTON_PATH_SKILLS_JSON = "skills.json";

// When rebuilding SD Card, remember auton config path

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