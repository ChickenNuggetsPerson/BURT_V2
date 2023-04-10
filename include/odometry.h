#pragma once
#include "vex.h"

static double PI = 3.14159;
static int tileWidth = 24; // Vex Tiles are 2ft across


// Positions are based on inches from the bottom left of the feild
struct Position {
    double x = 0;   // In Inches
    double y = 0;   // In Inches
    double rot = 0; // In Degrees
    Position(double xPos, double yPos, double rotation);
    Position(double xPos, double yPos);
    Position();
};



// Tiles are (0,0) to (5,5) from the bottom left to top right
// A cordinate of (0,0) will result in the center of the very most bottom left tile
// (5,5) is the very top right tile

/*
Feild Tile Cordinates

Y

5 | [] [] [] [] [] []
4 | [] [] [] [] [] []
3 | [] [] [] [] [] []
2 | [] [] [] [] [] []
1 | [] [] [] [] [] []
0 | [] [] [] [] [] []
    -- -- -- -- -- --
    0  1  2  3  4  5     X 

*/

// Since Vex games are allways based on the 6x6 tiles that are 2x2ft I plan on having a tile cord system too
struct TilePosition {
    double x = 0;   // In Tiles
    double y = 0;   // In Tiles
    double rot = 0; // In Degrees
    TilePosition(double xPos, double yPos, double rotation);
    TilePosition(double xPos, double yPos);
    TilePosition();
};


class OdometrySystem {
    private:

        vex::task trackingTask;

        Position currentPosition;
        TilePosition currentTilePosition;
       
        
        

        TilePosition posToTilePos(Position pos);
        Position tilePosToPos(TilePosition tilePos);

        void updateTilePos();


    public:

        OdometrySystem();

        bool isTracking = false;

        void restart();
        void restart(Position currentPos);
        void restart(TilePosition currentPos);

        void track();
        Position currentPos();
        TilePosition currentTilePos();

};

