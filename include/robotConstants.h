#include "pid.h"
#include "odometry.h"


// Misc Constants

static double PI = 3.14159; // PI variable used for all methods
static int tileWidth = 24; // Vex Tiles are 2ft across




// Define Robot Measurements 

static double wheelDiameter = 4;

static double encoderDist = 4.5;
static double backEncoderDist = 4.5; // use if we decide to use back encoders




// Odom Constants
static TilePosition ODOM_DEFAULT_RESET_POS(0, 0, 0);





// Auton Constants
static PIDConfig AUTON_TURNTO_PID_CONFIG(0.10, 0, 0.25);

static PIDConfig AUTON_GOTO_DRIVE_PID_CONFIG(0.2, 0.00, 0.00);
static PIDConfig AUTON_GOTO_TURN_PID_CONFIG(0.05, 0.00, 0.00);

static Position AUTON_START_POS1(0, 0, 0); // Configure these when the game releases
static Position AUTON_START_POS2(0, 0, 0);


