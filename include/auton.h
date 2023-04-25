#include "odometry.h"
#include <string>
#pragma once

// This is the file that defines the ai class

class ai {
  private:

    int teamColor;
    int startPos;

    bool loaded = false;

    OdometrySystem* odometrySystemPointer;

    double findNearestRot(double currentRot, double targetRot);
    double angleBetweenPoints(Position pos1, Position pos2);
    double distBetweenPoints(Position pos1, Position pos2);

  public:

    bool running = false;
    Position targetPos;

    std::string configFoler = "config/";
    std::string configFileType = ".txt";

    int totalConfigs = 8;
    std::string configNames[10] { // Random config names for now
      "test",
      "other",
      "ghjkk",
      "erwtw",
      "jrtyj",
      "egsdg",
      "asl;d",
      "yeet",
    };

    bool configStorage[10];

    bool configMenuStatus = false;

    ai(OdometrySystem* odometrySystemPointer);

    void init();

    bool getConfig(const char* configName);
    void saveConfig(const char* configName, bool value);

    bool isReady();

    Position getStartPos();

    void started();

    bool turnTo(double deg);
    bool gotoLoc(TilePosition pos);
    bool gotoLoc(Position pos);
};