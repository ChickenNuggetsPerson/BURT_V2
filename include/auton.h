#include "odometry.h"
#include <string>
#include <vector>
#pragma once

// This is the file that defines the ai class

class ai;

// Path Names
static const int AUTON_PATH_TEST = 0;
static const int AUTON_PATH_LEFT = 1;
static const int AUTON_PATH_RIGHT = 2;

// Type of movements
static const int AUTON_DELAY = 0;
static const int AUTON_DRIVE_DIST = 1;
static const int AUTON_GOTO = 2;
static const int AUTON_LONGGOTO = 3;
static const int AUTON_TURNTO = 4;
static const int AUTON_PICKUP = 5;
static const int AUTON_DROPOFF = 6;



struct autonMovement {
  int movementType = 0;
  Position pos;
  TilePosition tilePos;
  std::vector<TilePosition> drivePath;
  bool tilePosBool = false;
  double val = 0;
  autonMovement(int type, Position position) {
    movementType = type;
    tilePosBool = false;
    pos = position;
  };
  autonMovement(int type, TilePosition position) {
    movementType = type;
    tilePosBool = true;
    tilePos = position;
  };
  autonMovement(int type, double value) {
    movementType = type;
    val = value;
  };
  autonMovement(int type, std::vector<TilePosition> path) {
    movementType = type;
    drivePath = path;
  }
  autonMovement(int type) {
    movementType = type;
  }
  autonMovement() {};
};

class autonPath {
  private:
    ai* pointer;
    int currentStep = 0;
    int totalMovements = 0;
    autonMovement movements[10];
    bool runMovement(int movementNum);
  public:
    autonPath();
    autonPath(ai* autonPointer);
    void addMovement(autonMovement movement);
    void reset();
    bool step();
};

class ai {
  private:

    bool loaded = false;

    OdometrySystem* odometrySystemPointer;

    double findNearestRot(double currentRot, double targetRot);
    double angleBetweenPoints(Position pos1, Position pos2);
    double distBetweenPoints(Position pos1, Position pos2);

    Position target;

    autonPath path;

  public:

    bool running = false;
    int teamColor = 0;

    std::string configFoler = "autonConfig/";
    std::string configFileType = ".txt";

    int totalConfigs = 4;
    std::string configNames[10] { // Random config names for now
      "Red",
      "Blue",
      "Left",
      "Right",
    };

    bool configStorage[10];

    bool configMenuStatus = false;

    ai(OdometrySystem* odometrySystemPointer);

    void init();
    void constructPaths();

    bool getConfig(const char* configName);
    void saveConfig(const char* configName, bool value);

    bool isReady();

    Position getStartPos();
    Position getTargetPos();

    void started();
    void stop();

    bool driveDist(double dist);

    bool turnTo(double deg);
    bool turnTo(double deg, double turnTimeout);
    
    bool gotoLoc(TilePosition pos);
    bool gotoLoc(Position pos);
    
    bool longGoto(std::vector<TilePosition> pos);
    bool longGoto(std::vector<Position> pos);

    bool pickupAcorn();

    bool playPath(autonPath path);
};