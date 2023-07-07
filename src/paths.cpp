#include "paths.h"
#include "auton.h"
#include "displayer.h"
#include "robotConstants.h"

autonPath buildPath(int pathID, ai* autonPtr) {
    autonPath path = autonPath(autonPtr);
    
    if (pathID == AUTON_PATH_LEFT) {
        path.startPos = autonPtr->odometrySystemPointer->tilePosToPos(AUTON_START_LEFT);
        // Pick up matchload and then touch bar

        brainFancyDebug("Using AUTON_PATH_LEFT", vex::color::cyan);

        path.addMovement(autonMovement(AUTON_GOTO, TilePosition(0.5, 0.5, 225)));
        path.addMovement(autonMovement(AUTON_DRIVE_DIST, 10));
        path.addMovement(autonMovement(AUTON_PICKUP_ACORN));
        std::vector<TilePosition> tmpPath;
        tmpPath.push_back(TilePosition(1, 1));
        tmpPath.push_back(TilePosition(2, 1, 90));
        path.addMovement(autonMovement(AUTON_LONGGOTO, tmpPath));
    }

    if (pathID == AUTON_PATH_RIGHT) {
        path.startPos = autonPtr->odometrySystemPointer->tilePosToPos(AUTON_START_RIGHT);
        // Pick up triball, score it, maybe touch bar

        brainFancyDebug("Using AUTON_PATH_Right", vex::color::cyan);
        
        std::vector<TilePosition> tmpPath;
        tmpPath.push_back(TilePosition(4, 1));
        tmpPath.push_back(TilePosition(3.5, 1.5));
        tmpPath.push_back(TilePosition(3.5, 2, 90));
        path.addMovement(autonMovement(AUTON_LONGGOTO, tmpPath));

    }

    if (pathID == AUTON_PATH_SKILLS) {
        path.startPos = autonPtr->odometrySystemPointer->tilePosToPos(AUTON_START_SKILLS);
        brainFancyDebug("Using AUTON_PATH_SKILLS", vex::color::cyan);

        std::vector<TilePosition> leftPath;
        leftPath.push_back(TilePosition(1, 1));
        leftPath.push_back(TilePosition(1.5, 1.5));
        leftPath.push_back(TilePosition(1.5, 3.5));
        leftPath.push_back(TilePosition(1, 4));
        leftPath.push_back(TilePosition(1, 5));
        path.addMovement(autonMovement(AUTON_LONGGOTO, leftPath));
        path.addMovement(autonMovement(AUTON_GOTO, TilePosition(4, 5)));
        std::vector<TilePosition> rightPath;
        rightPath.push_back(TilePosition(4, 4));
        rightPath.push_back(TilePosition(3.5, 3.5));
        rightPath.push_back(TilePosition(3.5, 1.5));
        rightPath.push_back(TilePosition(4, 1));
        rightPath.push_back(TilePosition(4, 0));
        path.addMovement(autonMovement(AUTON_LONGGOTO, rightPath));
        path.addMovement(autonMovement(AUTON_GOTO, TilePosition(1, 0)));

    }

    if (pathID == AUTON_PATH_TEST) {
        path.startPos = autonPtr->odometrySystemPointer->tilePosToPos(TilePosition(0,0));
        brainFancyDebug("Using AUTON_PATH_TEST", vex::color::cyan);

        std::vector<TilePosition> tmpPath;
        tmpPath.push_back(TilePosition(0, 0));
        tmpPath.push_back(TilePosition(0, 1));
        tmpPath.push_back(TilePosition(1, 1));
        tmpPath.push_back(TilePosition(1, 0));
        tmpPath.push_back(TilePosition(0, 0));
        path.addMovement(autonMovement(AUTON_LONGGOTO, tmpPath));
    }

    return path;
}