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

    


  public:

    bool running = false;

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

};