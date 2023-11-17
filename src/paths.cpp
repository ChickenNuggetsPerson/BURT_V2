#include "paths.h"
#include "auton.h"
#include "displayer.h"
#include "robotConstants.h"

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
            
            path.addMovement(auton::autonMovement(AUTON_MOVE_ARM_CALIBRATE));
            path.addMovement(auton::autonMovement(AUTON_MOVE_GOTO, odom::TilePosition(1, 1)));
            path.addMovement(auton::autonMovement(AUTON_MOVE_GOTO, odom::TilePosition(1.25, 2, -90)));
            path.addMovement(auton::autonMovement(AUTON_MOVE_DELAY, 200));
            path.addMovement(auton::autonMovement(AUTON_MOVE_DRIVE_DIST, 20));
            path.addMovement(auton::autonMovement(AUTON_MOVE_DELAY, 200));
            path.addMovement(auton::autonMovement(AUTON_MOVE_TURNTO, -90));
            path.addMovement(auton::autonMovement(AUTON_MOVE_DELAY, 200));
            path.addMovement(auton::autonMovement(AUTON_MOVE_ARM_SET, 0));
            path.addMovement(auton::autonMovement(AUTON_MOVE_DELAY, 200));
            path.addMovement(auton::autonMovement(AUTON_MOVE_ARM_RELEASE));
            path.addMovement(auton::autonMovement(AUTON_MOVE_TURNTO, -85));
            path.addMovement(auton::autonMovement(AUTON_MOVE_DRIVE_REVERSE, 12));
            
            break;
        }
        case AUTON_PATH_RIGHT: {
            path.startPos = odom::tilePosToPos(AUTON_START_RIGHT);
            // Pick up triball, score it, maybe touch bar
            brainFancyDebug("Building AUTON_PATH_Right", vex::color::cyan);

            path.addMovement(auton::autonMovement(AUTON_MOVE_ARM_CALIBRATE));
            path.addMovement(auton::autonMovement(AUTON_MOVE_GOTO, odom::TilePosition(4, 1)));
            path.addMovement(auton::autonMovement(AUTON_MOVE_GOTO, odom::TilePosition(3.5, 2, 90)));
            path.addMovement(auton::autonMovement(AUTON_MOVE_DRIVE_DIST, 18));
            path.addMovement(auton::autonMovement(AUTON_MOVE_ARM_SET, 0));
            path.addMovement(auton::autonMovement(AUTON_MOVE_ARM_RELEASE));
            path.addMovement(auton::autonMovement(AUTON_MOVE_TURNTO, 80));
            path.addMovement(auton::autonMovement(AUTON_MOVE_DRIVE_REVERSE, 12));

            break;
        }          
        case AUTON_PATH_SKILLS: {
            path.startPos = odom::tilePosToPos(AUTON_START_SKILLS);
            brainFancyDebug("Building AUTON_PATH_SKILLS", vex::color::cyan);

            // Drive to Middle
            path.addMovement(auton::autonMovement(AUTON_MOVE_ARM_CALIBRATE));
            path.addMovement(auton::autonMovement(AUTON_MOVE_LONGGOTO, {
                odom::TilePosition(1, 1),
                odom::TilePosition(1.5, 2.5, -90)
            }));

            // Drive forward
            path.addMovement(auton::autonMovement(AUTON_MOVE_LONGGOTO, { 
                odom::TilePosition(0.45, 2.5),
                odom::TilePosition(0.45, 2.5),
                odom::TilePosition(0.45, 2.5),
            }));
        
            //  Drop off Acorn and back up
            path.addMovement(auton::autonMovement(AUTON_MOVE_DROPOFF_ACORN));
            path.addMovement(auton::autonMovement(AUTON_MOVE_TURNTO, -80));
            path.addMovement(auton::autonMovement(AUTON_MOVE_DRIVE_REVERSE, 24));
            path.addMovement(auton::autonMovement(AUTON_MOVE_ARM_SET, -10));

            // Go infront of matchload
            path.addMovement(auton::autonMovement(AUTON_MOVE_GOTO, odom::TilePosition(1, 0.75, -135)));
            path.addMovement(auton::autonMovement(AUTON_MOVE_DRIVE_DIST, 26));
            path.addMovement(auton::autonMovement(AUTON_MOVE_TURNTO, -120));

            // Touch bar and start catapulting
            path.addMovement(auton::autonMovement(AUTON_MOVE_ARM_SET, 200));
            path.addMovement(auton::autonMovement(AUTON_MOVE_ARM_RELEASE));
            path.addMovement(auton::autonMovement(AUTON_MOVE_CATAPULT, 12));

            // Backup
            path.addMovement(auton::autonMovement(AUTON_MOVE_ARM_SET, 0));
            path.addMovement(auton::autonMovement(AUTON_MOVE_ARM_RELEASE));
            path.addMovement(auton::autonMovement(AUTON_MOVE_DRIVE_REVERSE, 10));
            
            // Goto start pos
            path.addMovement(auton::autonMovement(AUTON_MOVE_GOTO, odom::TilePosition(1, 0)));

            // Drive across area
            path.addMovement(auton::autonMovement(AUTON_MOVE_GOTO, odom::TilePosition(4.25, 0.1)));
            path.addMovement(auton::autonMovement(AUTON_MOVE_GOTO, odom::TilePosition(4, 1)));
            path.addMovement(auton::autonMovement(AUTON_MOVE_GOTO, odom::TilePosition(3, 2.5)));

            // Open wings and push
            path.addMovement(auton::autonMovement(AUTON_MOVE_TURNTO, 90));
            path.addMovement(auton::autonMovement(AUTON_MOVE_WINGS_OPEN));
            path.addMovement(auton::autonMovement(AUTON_MOVE_DRIVE_DIST, 36));
            path.addMovement(auton::autonMovement(AUTON_MOVE_DRIVE_REVERSE, 12));
            
            break;
        }         
        case AUTON_PATH_TEST: {
            path.startPos = odom::tilePosToPos(TilePosition(0,0));
            brainFancyDebug("Building AUTON_PATH_TEST", vex::color::cyan);

            path.addMovement(auton::autonMovement(AUTON_MOVE_ARM_CALIBRATE));
            path.addMovement(auton::autonMovement(AUTON_MOVE_DELAY, 500));
            path.addMovement(auton::autonMovement(AUTON_MOVE_ARM_SET, 0));
            path.addMovement(auton::autonMovement(AUTON_MOVE_ARM_RELEASE));

            /*
            path.addMovement(auton::autonMovement(AUTON_MOVE_LONGGOTO, {
                odom::TilePosition(0, 0),
                odom::TilePosition(0, 3),
                odom::TilePosition(-3, 6),
                odom::TilePosition(-3, 15),
                odom::TilePosition(-4, 21),
                odom::TilePosition(-3, 26),
                odom::TilePosition(0, 21),
                odom::TilePosition(-3, 15),
                odom::TilePosition(-3, 4),
                odom::TilePosition(0, 0, 0)
            }));
            */
            
            break;
        }
    }

    return path;
}