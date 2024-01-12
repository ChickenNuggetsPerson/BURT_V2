#include "paths.h"
#include "auton.h"
#include "displayer.h"
#include "robotConstants.h"
#include "mainController.h"

using namespace auton;
using namespace odom;

autonPath buildPath(int pathID) {
    autonPath path = autonPath();
    

    // pathID = AUTON_PATH_TEST;

    switch (pathID) {
        case AUTON_PATH_LEFT: {
            path.startPos = odom::tilePosToPos(AUTON_START_LEFT);
            // Pick up matchload and then touch bar
            brainFancyDebug("Building AUTON_PATH_LEFT", vex::color::cyan);
            
            path.addMovement(auton::autonMovement(AUTON_MOVE_WING_SET_STATE, W_close));

            path.addMovement(auton::autonMovement(AUTON_MOVE_LONGGOTO, {
                odom::TilePosition(1.1, 1),
                odom::TilePosition(1.5, 1.75)
            }));

            path.addMovement(auton::autonMovement(AUTON_MOVE_TURNTO, -90));

            path.addMovement(auton::autonMovement(AUTON_MOVE_WING_SET_STATE, W_pos1));

            path.addMovement(auton::autonMovement(AUTON_MOVE_GOTO, odom::TilePosition(0.92, 1.7)));
            
            path.addMovement(auton::autonMovement(AUTON_MOVE_TURNTO, -90));

            path.addMovement(auton::autonMovement(AUTON_MOVE_DRIVE_REVERSE, 10));

            path.addMovement(auton::autonMovement(AUTON_MOVE_GOTO, odom::TilePosition(1.8, 1, 178)));

            path.addMovement(auton::autonMovement(AUTON_MOVE_WING_SET_STATE, W_LeftAutonPoleTouch));

            // path.addMovement(auton::autonMovement(AUTON_MOVE_DRIVE_DIST, 10));

            path.addMovement(auton::autonMovement(AUTON_MOVE_GOTO, odom::TilePosition(2.4, 0, 180)));


            break;
        }
        case AUTON_PATH_RIGHT: {
            path.startPos = odom::tilePosToPos(AUTON_START_RIGHT);
            // Pick up triball, score it, maybe touch bar
            brainFancyDebug("Building AUTON_PATH_Right", vex::color::cyan);

            path.addMovement(auton::autonMovement(AUTON_MOVE_WING_SET_STATE, W_close));

            path.addMovement(auton::autonMovement(AUTON_MOVE_LONGGOTO, {
                odom::TilePosition(4, 0.1),
                odom::TilePosition(4, 0.4),
                odom::TilePosition(5, 1, 0),
            }));
            path.addMovement(auton::autonMovement(AUTON_MOVE_TURNTO, 0));
            path.addMovement(auton::autonMovement(AUTON_MOVE_WING_SET_STATE, W_pos1));
            path.addMovement(auton::autonMovement(AUTON_MOVE_DRIVE_DIST, 10));
            path.addMovement(auton::autonMovement(AUTON_MOVE_DRIVE_REVERSE, 20));


            /*
            path.addMovement(auton::autonMovement(AUTON_MOVE_GOTO, odom::TilePosition(4, 1)));
            path.addMovement(auton::autonMovement(AUTON_MOVE_GOTO, odom::TilePosition(3.5, 2, 90)));
            path.addMovement(auton::autonMovement(AUTON_MOVE_DRIVE_DIST, 18));
            path.addMovement(auton::autonMovement(AUTON_MOVE_ARM_SET, 0));
            path.addMovement(auton::autonMovement(AUTON_MOVE_ARM_RELEASE));
            path.addMovement(auton::autonMovement(AUTON_MOVE_TURNTO, 80));
            path.addMovement(auton::autonMovement(AUTON_MOVE_DRIVE_REVERSE, 12));
            */

            break;
        }          
        case AUTON_PATH_SKILLS: {
            path.startPos = odom::tilePosToPos(AUTON_START_SKILLS);
            brainFancyDebug("Building AUTON_PATH_SKILLS", vex::color::cyan);
            
            // Go infront of matchload
            path.addMovement(auton::autonMovement(AUTON_MOVE_GOTO, odom::TilePosition(1, 0.75, -135)));
            path.addMovement(auton::autonMovement(AUTON_MOVE_DRIVE_DIST, 26));
            path.addMovement(auton::autonMovement(AUTON_MOVE_TURNTO, -120));

            // Touch bar and start catapulting
            path.addMovement(auton::autonMovement(AUTON_MOVE_CATAPULT, 70));

            // Backup
            // path.addMovement(auton::autonMovement(AUTON_MOVE_DRIVE_REVERSE, 10));
            
            // // Goto start pos
            // path.addMovement(auton::autonMovement(AUTON_MOVE_GOTO, odom::TilePosition(0.9, -1)));

            // // Drive across area
            // path.addMovement(auton::autonMovement(AUTON_MOVE_GOTO, odom::TilePosition(5, -0.1)));
            // path.addMovement(auton::autonMovement(AUTON_MOVE_GOTO, odom::TilePosition(4, 1)));
            // path.addMovement(auton::autonMovement(AUTON_MOVE_GOTO, odom::TilePosition(3, 2.5)));

            // // Open wings and push
            // path.addMovement(auton::autonMovement(AUTON_MOVE_TURNTO, 90));
            // path.addMovement(auton::autonMovement(AUTON_MOVE_WING_SET_STATE, W_pos2));
            // path.addMovement(auton::autonMovement(AUTON_MOVE_DRIVE_DIST, 36));
            // path.addMovement(auton::autonMovement(AUTON_MOVE_DRIVE_REVERSE, 12));
            
            break;
        }         
        case AUTON_PATH_TEST: {
            path.startPos = odom::tilePosToPos(TilePosition(1,0));
            brainFancyDebug("Building AUTON_PATH_TEST", vex::color::cyan);

            
            path.addMovement(auton::autonMovement(AUTON_MOVE_LONGGOTO, {
                odom::TilePosition(4, 0),
                odom::TilePosition(4, 0),
                odom::TilePosition(4, 0),
            }));
            
            
            break;
        }
    }

    return path;
}