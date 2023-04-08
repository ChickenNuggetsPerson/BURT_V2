#pragma once
#include "vex.h"

static double PI = 3.14159;

struct Position {
    double x = 0;
    double y = 0;
    double rot = 0; // In Radians
};


class OdometrySystem {
    private:

        vex::task trackingTask;

        double xPos = 0;
        double yPos = 0;
        double rot = 0; // In Radians
       

    public:

        OdometrySystem();

        bool isTracking = false;

        void restart(Position currentPos = Position());
        void track();
        Position currentPos();

};

