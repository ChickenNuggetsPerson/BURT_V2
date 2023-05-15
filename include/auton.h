#include "odometry.h"
#include <string>
#pragma once

// This is the file that defines the ai class

class ai {
  private:

    bool loaded = false;

    OdometrySystem* odometrySystemPointer;

    double findNearestRot(double currentRot, double targetRot);
    double angleBetweenPoints(Position pos1, Position pos2);
    double distBetweenPoints(Position pos1, Position pos2);

    Position target;

  public:

    bool running = false;
    int teamColor = 0;

    std::string configFoler = "autonConfig/";
    std::string configFileType = ".txt";

    int totalConfigs = 4;
    std::string configNames[10] { // Random config names for now
      "Left",
      "Right",
      "Red",
      "Blue",
    };

    bool configStorage[10];

    bool configMenuStatus = false;

    ai(OdometrySystem* odometrySystemPointer);

    void init();

    bool getConfig(const char* configName);
    void saveConfig(const char* configName, bool value);

    bool isReady();

    Position getStartPos();
    Position getTargetPos();

    void started();
    void stop();

    bool turnTo(double deg);
    bool turnTo(double deg, double turnTimeout);
    bool gotoLoc(TilePosition pos);
    bool gotoLoc(Position pos);
};