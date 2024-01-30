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
            path.startPos = tilePosToPos(AUTON_START_LEFT);
            // Pick up matchload and then touch bar
            brainFancyDebug("Building AUTON_PATH_LEFT", vex::color::cyan);
            
            path.add(autonMovement(AUTON_MOVE_WING_SET_STATE, W_close));

            path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(0, 0.7, 0)));
            path.add(autonMovement(AUTON_MOVE_WING_SET_STATE, W_pos1));

            path.add(autonMovement(AUTON_MOVE_DRIVE_DIST, 6));
            path.add(autonMovement(AUTON_MOVE_DRIVE_REVERSE, 6));

            path.add(autonMovement(AUTON_MOVE_TURNTO, 170));

            path.add(autonMovement(AUTON_MOVE_WING_SET_STATE, W_LeftAutonSwipeTriball));
            path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(0.3, 0.15, 90)));
            path.add(autonMovement(AUTON_MOVE_WING_SET_STATE, W_loose));

            path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(1, -0.1)));
            // path.add(autonMovement(AUTON_MOVE_WING_SET_STATE, W_loose));
            path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(1.78, 0)));

            path.add(autonMovement(AUTON_MOVE_DELAY, 200));

            path.add(autonMovement(AUTON_MOVE_CATAPULT, 1));

            path.add(autonMovement(AUTON_MOVE_WING_SET_STATE, W_LeftAutonPoleTouch));


            // path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(1.1, 1)));
            // path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(1.5, 1.75, -90)));

            // path.add(autonMovement(AUTON_MOVE_WING_SET_STATE, W_pos1));

            // path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(0.92, 1.7)));
            
            // path.add(autonMovement(AUTON_MOVE_TURNTO, -90));

            // path.add(autonMovement(AUTON_MOVE_DRIVE_REVERSE, 10));

            // path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(1.8, 1, 178)));

            // path.add(autonMovement(AUTON_MOVE_WING_SET_STATE, W_LeftAutonPoleTouch));

            // path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(2.4, 0, 180)));


            break;
        }
        case AUTON_PATH_RIGHT: {
            path.startPos = tilePosToPos(AUTON_START_RIGHT);
            // Pick up triball, score it, maybe touch bar
            brainFancyDebug("Building AUTON_PATH_Right", vex::color::cyan);

            path.add(autonMovement(AUTON_MOVE_WING_SET_STATE, W_close));


            // TODO: Add auton win-point
            // path.add(autonMovement(AUTON_MOVE_LONGGOTO, {
            //     TilePosition(4, 0.1),
            //     TilePosition(4, 0.4),
            //     TilePosition(5, 1, 0),
            // }));
            
            // path.add(autonMovement(AUTON_MOVE_TURNTO, 0));
            // path.add(autonMovement(AUTON_MOVE_WING_SET_STATE, W_pos1));
            // path.add(autonMovement(AUTON_MOVE_DRIVE_DIST, 10));
            // path.add(autonMovement(AUTON_MOVE_DRIVE_REVERSE, 20));


            // Score preload
            path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(4.9, 1, 0)));
            path.add(autonMovement(AUTON_MOVE_DRIVE_DIST, 12));
            path.add(autonMovement(AUTON_MOVE_WING_SET_STATE, W_pos3));
            path.add(autonMovement(AUTON_MOVE_DRIVE_REVERSE, 10));

            // Grab another triball
            path.add(autonMovement(AUTON_MOVE_TURNTO, -90));
            // path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(4, 1)));
            path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(2.9, 1.25)));
            path.add(autonMovement(AUTON_MOVE_WING_SET_STATE, W_close));
            // path.add(autonMovement(AUTON_MOVE_TURNTO, 90));

            // Score
            path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(4.5, 2)));
            path.add(autonMovement(AUTON_MOVE_WING_SET_STATE, W_pos3));
            path.add(autonMovement(AUTON_MOVE_DRIVE_REVERSE, 10));

            /*
            path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(4, 1)));
            path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(3.5, 2, 90)));
            path.add(autonMovement(AUTON_MOVE_DRIVE_DIST, 18));
            path.add(autonMovement(AUTON_MOVE_ARM_SET, 0));
            path.add(autonMovement(AUTON_MOVE_ARM_RELEASE));
            path.add(autonMovement(AUTON_MOVE_TURNTO, 80));
            path.add(autonMovement(AUTON_MOVE_DRIVE_REVERSE, 12));
            */

            break;
        }          
        case AUTON_PATH_SKILLS: {
            path.startPos = tilePosToPos(AUTON_START_SKILLS);
            brainFancyDebug("Building AUTON_PATH_SKILLS", vex::color::cyan);
            
            // Go infront of matchload
            path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(1, 0.75, -135)));
            path.add(autonMovement(AUTON_MOVE_DRIVE_DIST, 26));
            path.add(autonMovement(AUTON_MOVE_TURNTO, -102));

            // Touch bar and start catapulting
            path.add(autonMovement(AUTON_MOVE_CATAPULT, 35));

            // Backup
            path.add(autonMovement(AUTON_MOVE_DRIVE_REVERSE, 10));
            
            // // Goto start pos
            path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(0.9, -1)));

            // // Drive across area
            path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(4.75, -0.1)));
            // path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(4, 1)));
            path.add(autonMovement(AUTON_MOVE_GOTO, TilePosition(3, 2.25)));

            // // Open wings and push
            path.add(autonMovement(AUTON_MOVE_TURNTO, 90));
            path.add(autonMovement(AUTON_MOVE_WING_SET_STATE, W_pos2));
            path.add(autonMovement(AUTON_MOVE_DRIVE_DIST, 36));
            path.add(autonMovement(AUTON_MOVE_DRIVE_REVERSE, 12));
            
            break;
        }         
        case AUTON_PATH_TEST: {
            path.startPos = tilePosToPos(TilePosition(1,0));
            brainFancyDebug("Building AUTON_PATH_TEST", vex::color::cyan);

            // path.add(autonMovement(AUTON_MOVE_LONGGOTO, {
            //     TilePosition(4, 0),
            //     TilePosition(4, 0),
            //     TilePosition(4, 0),
            // }));

            path.add(autonMovement(AUTON_MOVE_DRIVE_DIST, 24));
            
            break;
        }
    }

    return path;
}