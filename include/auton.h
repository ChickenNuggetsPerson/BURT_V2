
#include <string>
#pragma once

// This is the file that defines the ai class

class ai {
  private:

    int teamColor;
    int startPos;

    bool loaded = false;

  public:

    std::string configFoler = "config/";

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

    void init();

    bool getConfig(const char* configName);
    void saveConfig(const char* configName, bool value);

    bool isReady();

    void started();

};