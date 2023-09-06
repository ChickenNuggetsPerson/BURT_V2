/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       hayden                                                    */
/*    Created:      3/15/2023, 8:15:25 PM                                     */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#include "vex.h"
#include "robotConfig.h"

using namespace vex;


using std::cout;
using std::endl;


int main() {

  cout << endl << endl << endl;

  Competition.bStopTasksBetweenModes = true;
  
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  
  pre_auton();

  whenStarted();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
  
}
