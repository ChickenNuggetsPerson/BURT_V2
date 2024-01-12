#pragma once

#include "vex.h"
#include "misc.h"

namespace odom {

    static int tileWidth = 24; // Vex Tiles are 2ft across

    struct TilePosition; // Forward Declaration
    
    // Positions are based on inches from the bottom left of the feild
    struct Position {
        double x = 0.00;   // In Inches
        double y = 0.00;   // In Inches
        double rot = 0.00; 
        Position(double xPos, double yPos, double rotation);
        Position(double xPos, double yPos);
        Position(TilePosition pos);
        Position();
    };
    

    // Positions based on tiles
    struct TilePosition {
        double x = 0;   // In Tiles
        double y = 0;   // In Tiles
        double rot = 0.00; // In Degrees
        TilePosition(double xPos, double yPos, double rotation);
        TilePosition(double xPos, double yPos);
        TilePosition(Position pos);
        TilePosition();
    };


    // Used by the odometry system for keeping track of changes in location
    struct odomRawData {
        double rightEncoder = 0.00;
        double leftEncoder = 0.00;
        double heading = 0.00;
        double locX = 0.00;
        double locY = 0.00;

        double deltaRight;
        double deltaLeft;
        double deltaHeading;
    };

    // Position conversions
    TilePosition posToTilePos(Position pos);
    Position tilePosToPos(TilePosition tilePos);

    // The main system for tracking the position of the robot over time
    // It is required for the autonomous to stay consistent over time
    class OdometrySystem {
        private:

            // The main task for tracking the robot
            vex::task trackingTask;

            // The internal storage for robot location
            double globalX = 0.00;
            double globalY = 0.00;
            double globalRot = 0.00;

            // Internal storage for the tile position
            TilePosition currentTilePosition;
            void updateTilePos();

            misc::ValueAverager<10> inertialAverager = misc::ValueAverager<10>();

            // Tracks changes in position between odom steps
            odomRawData lastData; 
            odomRawData getChanges(odomRawData data);


        public:

            OdometrySystem();

            bool isTracking = false;
            bool firstTime = true;
            bool usingDrive = false;

            void restart();
            void restart(Position currentPos);
            void restart(TilePosition currentPos);

            void setPos(Position newPos);
            void setPos(TilePosition newPos);

            void track();
            Position currentPos();
            TilePosition currentTilePos();
            
            void resetEncoders();
    };

}


