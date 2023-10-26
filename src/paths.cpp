#include "paths.h"
#include "auton.h"
#include "displayer.h"
#include "robotConstants.h"

using namespace auton;
using namespace odom;

autonPath buildPath(int pathID) {
    autonPath path = autonPath();
    
    switch (pathID) {
        case AUTON_PATH_LEFT: {
            path.startPos = odom::tilePosToPos(AUTON_START_LEFT);
            // Pick up matchload and then touch bar
            brainFancyDebug("Building AUTON_PATH_LEFT", vex::color::cyan);
            path.addMovement(autonMovement(AUTON_MOVE_GOTO, TilePosition(0.5, 0.5, 225)));
            path.addMovement(autonMovement(AUTON_MOVE_DRIVE_DIST, 10));
            path.addMovement(autonMovement(AUTON_MOVE_PICKUP_ACORN));
            std::vector<TilePosition> tmpPath;
            tmpPath.push_back(TilePosition(1, 1));
            tmpPath.push_back(TilePosition(2, 1, 90));
            path.addMovement(autonMovement(AUTON_MOVE_LONGGOTO, tmpPath));
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
            path.addMovement(auton::autonMovement(AUTON_MOVE_DROPOFF_ACORN));
            path.addMovement(auton::autonMovement(AUTON_MOVE_TURNTO, 80));
            path.addMovement(auton::autonMovement(AUTON_MOVE_DRIVE_REVERSE, 12));

            break;
        }          
        case AUTON_PATH_SKILLS: {
            path.startPos = odom::tilePosToPos(AUTON_START_SKILLS);
            brainFancyDebug("Building AUTON_PATH_SKILLS", vex::color::cyan);

            std::vector<TilePosition> leftPath;
            leftPath.push_back(TilePosition(1, 1));
            leftPath.push_back(TilePosition(1.5, 1.5));
            leftPath.push_back(TilePosition(1.5, 3.5));
            leftPath.push_back(TilePosition(1, 4));
            leftPath.push_back(TilePosition(1, 5));
            path.addMovement(autonMovement(AUTON_MOVE_LONGGOTO, leftPath));
            path.addMovement(autonMovement(AUTON_MOVE_GOTO, TilePosition(4, 5)));
            std::vector<TilePosition> rightPath;
            rightPath.push_back(TilePosition(4, 4));
            rightPath.push_back(TilePosition(3.5, 3.5));
            rightPath.push_back(TilePosition(3.5, 1.5));
            rightPath.push_back(TilePosition(4, 1));
            rightPath.push_back(TilePosition(4, 0));
            path.addMovement(autonMovement(AUTON_MOVE_LONGGOTO, rightPath));
            path.addMovement(autonMovement(AUTON_MOVE_GOTO, TilePosition(1, 0)));
            break;
        }         
        case AUTON_PATH_TEST: {
            path.startPos = odom::tilePosToPos(TilePosition(0,0));
            brainFancyDebug("Building AUTON_PATH_TEST", vex::color::cyan);

            std::vector<TilePosition> tmpPath;
            tmpPath.push_back(TilePosition(0, 0));
            tmpPath.push_back(TilePosition(0, 1));
            tmpPath.push_back(TilePosition(1, 1));
            tmpPath.push_back(TilePosition(1, 0));
            tmpPath.push_back(TilePosition(0, 0));
            path.addMovement(autonMovement(AUTON_MOVE_LONGGOTO, tmpPath));
            break;
        }
    }

    return path;
}