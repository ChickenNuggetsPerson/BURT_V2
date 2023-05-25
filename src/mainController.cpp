
#include "robotConfig.h"
#include "controllerDisplayerAPI.h"
using namespace vex;


// Controller Stuff

int motorFL = 0;
int motorFR = 0;
int motorBL = 0;
int motorBR = 0;

int leftFB = 0;
int rightFB = 0;

int turn = 0;

int controllerTask() {

  std::cout << "Starting controller" << std::endl;

  // process the controller input every 20 milliseconds
  // update the motors based on the input values

  leftMotorA.spin(fwd);
  leftMotorB.spin(fwd);
  rightMotorA.spin(fwd);
  rightMotorB.spin(fwd);

  leftMotorA.setVelocity(0, percent);
  leftMotorB.setVelocity(0, percent);
  rightMotorA.setVelocity(0, percent);
  rightMotorB.setVelocity(0, percent);


  // Main driving loop
  while(true) {

    if(Competition.isDriverControl()) {

      if (!inertialSensor.isCalibrating()) {
        leftFB = mainController.Axis3.position();
        rightFB = mainController.Axis3.position();

        turn = mainController.Axis1.position();
      }

    }
    

    motorFL = leftFB + turn;
    motorFR = rightFB - turn;

    motorBL = leftFB + turn;
    motorBR = rightFB - turn;

    if ( !botAI.running ) {

      leftMotorA.spin(fwd);
      leftMotorB.spin(fwd);
      rightMotorA.spin(fwd);
      rightMotorB.spin(fwd);

      leftMotorA.setVelocity(motorFL, percent);
      leftMotorB.setVelocity(motorBL, percent);
      rightMotorA.setVelocity(motorFR, percent);
      rightMotorB.setVelocity(motorBR, percent);
      
    }
    
    // wait before repeating the process
    vex::wait(20, msec);
  }
  return 0;
}

// The main controller rendering task
void mainControllerRender() {
  mainController.Screen.clearScreen();
  mainController.Screen.setCursor(1, 1);

  Position currentPos = Odometry.currentPos();

  mainController.Screen.print(currentPos.x);
  mainController.Screen.newLine();
  mainController.Screen.print(currentPos.y);
  mainController.Screen.newLine();
  mainController.Screen.print(limitAngle(radToDegree(currentPos.rot)));            

  // Test Controller Overlay Choice
  if (mainController.ButtonA.pressing()) {
    bool chosen = displayOverlay(ControllerOverlay("Choose", "Launch", "Other"), &mainController);

    if (chosen) {
      std::cout << "Other" << std::endl;
    } else {
      std::cout << "Launch" << std::endl;
    }

  }

  // Test Controller Choose from list
  if (mainController.ButtonB.pressing()) {
    std::vector <const char *> options;

    options.push_back("(0, 0)");
    options.push_back("(1, 1)");
    options.push_back("(2, 5)");
    options.push_back("(6, 6)");
    options.push_back("(3, 2)");

    int chosen = pickOption(options, &mainController);

    if (chosen == 0) { botAI.gotoLoc(TilePosition(0, 0)); }
    if (chosen == 1) { botAI.gotoLoc(TilePosition(1, 1)); }
    if (chosen == 2) { botAI.gotoLoc(TilePosition(2, 5)); }
    if (chosen == 3) { botAI.gotoLoc(TilePosition(6, 6)); }
    if (chosen == 4) { botAI.gotoLoc(TilePosition(3, 2)); }


  }

}
