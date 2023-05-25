#include "paths.h"
#include "auton.h"

autonPath buildPath(int pathID, ai* autonPtr) {
    autonPath path = autonPath(autonPtr);
    
    if (pathID == AUTON_PATH_LEFT) {

        // Pick up matchload and then touch bar

        path.addMovement(autonMovement(AUTON_GOTO, TilePosition(0.5, 0.5, 225)));
        path.addMovement(autonMovement(AUTON_DRIVE_DIST, 10));
        path.addMovement(autonMovement(AUTON_PICKUP));
        path.addMovement(autonMovement(AUTON_GOTO, TilePosition(1, 1, NAN)));
        path.addMovement(autonMovement(AUTON_GOTO, TilePosition(2, 1, 90)));
    }

    if (pathID == AUTON_PATH_RIGHT) {

        // Pick up triball, score it, maybe touch bar



    }

    return path;
}