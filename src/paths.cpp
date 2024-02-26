#include "paths.h"
#include "auton.h"
#include "displayer.h"
#include "robotConstants.h"
#include "mainController.h"

using namespace auton;
using namespace odom;

#define releaseIntake() { path.add(autonMovement(AUTON_MOVE_INTAKE_SET, -1)); path.add(autonMovement(AUTON_MOVE_DELAY, 200)); path.add(autonMovement(AUTON_MOVE_INTAKE_SET, 0)); }

autonPath buildPath(int pathID, bool isFinals) {
    autonPath path = autonPath();
    
    // pathID = AUTON_PATH_TEST;

    switch (pathID * (isFinals ? 10 : 1)) {
        case AUTON_PATH_LEFT * 10: // Finals Left route
        case AUTON_PATH_LEFT: {
            path.startPos = tilePosToPos(AUTON_START_LEFT);
            // Pick up matchload and then touch bar
            brainFancyDebug("Building AUTON_PATH_LEFT", vex::color::cyan);

            releaseIntake()

            // Pickup Triball
            path.add(autonMovement(AUTON_MOVE_INTAKE_SET, 1));
            path.add(autonMovement(AUTON_MOVE_DELAY, 400));
            path.add(autonMovement(AUTON_MOVE_INTAKE_SET, 0));

            // Score Triball
            // path.add(autonMovement(AUTON_MOVE_WING_SET_STATE, W_L_Open));
            path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(-0.2, 1.1)));
            path.add(autonMovement(AUTON_MOVE_TURNTO, 0));

            path.add(autonMovement(AUTON_MOVE_WING_SET_STATE, W_Close));
            path.add(autonMovement(AUTON_MOVE_INTAKE_SET, -1));

    
            path.add(autonMovement(AUTON_MOVE_DELAY, 1000));
            path.add(autonMovement(AUTON_MOVE_INTAKE_SET, 0));
            path.add(autonMovement(AUTON_MOVE_DRIVE_REVERSE, 8));

            // Face tribal
            path.add(autonMovement(AUTON_MOVE_TURNTO, 150));

            // Swipe Triball
            path.add(autonMovement(AUTON_MOVE_DRIVE_DIST, 24));
            path.add(autonMovement(AUTON_MOVE_WING_SET_STATE, W_L_Open));
            path.add(autonMovement(AUTON_MOVE_TURNTO, 90));
            path.add(autonMovement(AUTON_MOVE_WING_SET_STATE, W_Close));
            path.add(autonMovement(AUTON_MOVE_DRIVE_REVERSE, 4));

            // Finish
            path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(1, 1.1)));
            path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(1.9, 0.9)));
            path.add(autonMovement(AUTON_MOVE_WING_SET_STATE, W_L_Open));
            path.add(autonMovement(AUTON_MOVE_TURNTO, 20));


            break;
        }
        case AUTON_PATH_RIGHT * 10: { // Finals Right route 
            path.startPos = tilePosToPos(AUTON_START_RIGHT);
            // Pick up triball, score it, maybe touch bar
            brainFancyDebug("Building AUTON_PATH_Right_Finals", vex::color::cyan);

            // FINISH THIS LATER, MAKE MEASURE STICK
            releaseIntake()

            path.add(autonMovement(AUTON_MOVE_INTAKE_SET, 1));
            path.add(autonMovement(AUTON_MOVE_DELAY, 400));
            path.add(autonMovement(AUTON_MOVE_INTAKE_SET, 0));


            // Score matchload
            path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(5, 1, 0)));
            path.add(autonMovement(AUTON_MOVE_INTAKE_SET, -1));
            path.add(autonMovement(AUTON_MOVE_DRIVE_DIST, 12));
            path.add(autonMovement(AUTON_MOVE_DRIVE_REVERSE, 12));

            // Pickup second triball
            path.add(autonMovement(AUTON_MOVE_INTAKE_SET, 1));
            path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(2.8, 1.5)));
            path.add(autonMovement(AUTON_MOVE_DRIVE_REVERSE, 4));
            path.add(autonMovement(AUTON_MOVE_INTAKE_SET, 0));

            // Score second triball
            path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(3.9, 2, 90)));
            path.add(autonMovement(AUTON_MOVE_INTAKE_SET, -1));
            path.add(autonMovement(AUTON_MOVE_DRIVE_DIST, 6));
            path.add(autonMovement(AUTON_MOVE_DRIVE_REVERSE, 4));

            path.add(autonMovement(AUTON_MOVE_INTAKE_SET, 0));

            
            break;
        }
        case AUTON_PATH_RIGHT: { //  Normal Right route
            path.startPos = tilePosToPos(AUTON_START_RIGHT);
            // Pick up triball, score it, maybe touch bar
            brainFancyDebug("Building AUTON_PATH_Right", vex::color::cyan);

            // FINISH THIS LATER, MAKE MEASURE STICK
            releaseIntake()

            path.add(autonMovement(AUTON_MOVE_INTAKE_SET, 1));
            path.add(autonMovement(AUTON_MOVE_DELAY, 400));
            path.add(autonMovement(AUTON_MOVE_INTAKE_SET, 0));

            // path.add(autonMovement(AUTON_MOVE_DRIVE_REVERSE, 30));
            // path.add(autonMovement(AUTON_MOVE_DRIVE_DIST, 10));
            path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(5, 1, 0)));
            path.add(autonMovement(AUTON_MOVE_INTAKE_SET, -1));
            path.add(autonMovement(AUTON_MOVE_DRIVE_DIST, 12));
            path.add(autonMovement(AUTON_MOVE_DRIVE_REVERSE, 6));
            path.add(autonMovement(AUTON_MOVE_INTAKE_SET, 0));

            path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(3.9, 1.3)));
            path.add(autonMovement(AUTON_MOVE_WING_SET_STATE, W_R_Open));
            path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(3.2, 0.8, 0)));
            
            break;
        }          
        case AUTON_PATH_SKILLS * 10: // Finals skils route, this will just pass through to skills
        case AUTON_PATH_SKILLS: {
            path.startPos = tilePosToPos(AUTON_START_SKILLS);
            brainFancyDebug("Building AUTON_PATH_SKILLS", vex::color::cyan);

            releaseIntake()
            
            // Go infront of matchload
            path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(0.9, 0.6, -120)));
            path.add(autonMovement(AUTON_MOVE_DRIVE_DIST, 35));
            path.add(autonMovement(AUTON_MOVE_TURNTO, -110));

            // Touch bar and start catapulting
            path.add(autonMovement(AUTON_MOVE_CATAPULT, 30)); // 35

            // Backup
            path.add(autonMovement(AUTON_MOVE_DRIVE_REVERSE, 12));
            
            // // Goto start pos
            path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(0.9, -1)));
            path.add(autonMovement(AUTON_MOVE_TURNTO, 90));

            // // Drive across area
            path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(4.25, -0.2)));
            path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(4, 0.5)));
            path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(3, 2)));

            // // Open wings and push
            path.add(autonMovement(AUTON_MOVE_TURNTO, -110));
            path.add(autonMovement(AUTON_MOVE_DRIVE_DIST, 6));

            // path.add(autonMovement(AUTON_MOVE_TURNTO, -135));
            path.add(autonMovement(AUTON_MOVE_WING_SET_STATE, W_All_Open));
            path.add(autonMovement(AUTON_MOVE_DRIVE_REVERSE, 50));
            path.add(autonMovement(AUTON_MOVE_DRIVE_DIST, 30));
            path.add(autonMovement(AUTON_MOVE_WING_SET_STATE, W_Close));

            path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(2.5, 3.5)));
            path.add(autonMovement(AUTON_MOVE_TURNTO, -60));

            path.add(autonMovement(AUTON_MOVE_WING_SET_STATE, W_All_Open));
            path.add(autonMovement(AUTON_MOVE_DRIVE_REVERSE, 50));

            path.add(autonMovement(AUTON_MOVE_DRIVE_DIST, 6));
            
            break;
        }         
        case AUTON_PATH_TEST: {
            path.startPos = tilePosToPos(TilePosition(1,0));
            brainFancyDebug("Building AUTON_PATH_TEST", vex::color::cyan);
            
            releaseIntake()

            path.add(autonMovement(AUTON_MOVE_DRIVE_DIST, 24));
            
            break;
        }
    }

    return path;
}