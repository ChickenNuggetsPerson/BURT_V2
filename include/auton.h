#pragma once

#include "odometry.h"
#include "robotConstants.h"
#include <string>
#include <vector>

// This is the file that defines the ai class


// Path Names
static const int AUTON_PATH_TEST = 0;
static const int AUTON_PATH_LEFT = 1;
static const int AUTON_PATH_RIGHT = 2;
static const int AUTON_PATH_SKILLS = 3;

// Type of movements
// Make sure to keep these the same with the online editor
static const int AUTON_MOVE_END = -1;
static const int AUTON_MOVE_DELAY = 0;
static const int AUTON_MOVE_DRIVE_DIST = 1;
static const int AUTON_MOVE_GOTO = 2;
static const int AUTON_MOVE_LONGGOTO = 3;
static const int AUTON_MOVE_TURNTO = 4;
// static const int AUTON_MOVE_PICKUP_ACORN = 5;   
// static const int AUTON_MOVE_DROPOFF_ACORN = 6;
static const int AUTON_MOVE_DRIVE_REVERSE = 7;
// static const int AUTON_MOVE_ARM_SET = 8;     
// static const int AUTON_MOVE_ARM_RELEASE = 9;   
// static const int AUTON_MOVE_WINGS_OPEN = 10;
// static const int AUTON_MOVE_WINGS_CLOSE = 11;
static const int AUTON_MOVE_CATAPULT = 12;
// static const int AUTON_MOVE_ARM_CALIBRATE = 13;

static const int AUTON_MOVE_WING_SET_STATE = 14;
static const int AUTON_MOVE_INTAKE_SET = 15;

namespace auton {

  class AutonSystem;
  class aiQueueSystem;

  struct autonMovement {
    int movementType = 0;
    odom::Position pos;
    odom::TilePosition tilePos;
    std::vector<odom::TilePosition> drivePath;
    bool tilePosBool = false;
    double val = 0;
    autonMovement(int type, odom::Position position) {
      movementType = type;
      tilePosBool = false;
      pos = position;
    };
    autonMovement(int type, odom::TilePosition position) {
      movementType = type;
      tilePosBool = true;
      tilePos = position;
    };
    autonMovement(int type, double value) {
      movementType = type;
      val = value;
    };
    autonMovement(int type, std::vector<odom::TilePosition> path) {
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
      std::vector<autonMovement> movements;
    public:
      odom::Position startPos;
      autonPath() {};
      
      inline void add(autonMovement movement) { movements.push_back(movement); };
      inline int size() { return movements.size(); };
      inline autonMovement operator[](int i) { return i >= movements.size() ? autonMovement(AUTON_MOVE_END) : movements.at(i); };
  };


  struct autonConfig {
    std::string id;

    std::string name;
    bool status = false;

    std::string trueName;
    bool twoVals = false;

    vex::color falseColor = vex::color(168, 0, 0);
    vex::color trueColor = vex::color(0, 168, 0);
    autonConfig(const char* id, const char* name) { this->name = std::string(name); this->id = std::string(id);}
    autonConfig(const char* id, const char* name, bool defaultVal) { this->name = name; this->status = defaultVal; this->id = id;}
    autonConfig(const char* id, const char* falseName, const char* trueName, bool defaultVal) {
      this->name = std::string(falseName);
      this->trueName = std::string(trueName);
      this->twoVals = true;
      this->status = defaultVal;
      this->id = id;
    }
    autonConfig(const char* id, const char* falseName, const char* trueName, bool defaultVal, vex::color falseColor, vex::color trueColor) {
      this->name = std::string(falseName);
      this->trueName = std::string(trueName);
      this->twoVals = true;
      this->status = defaultVal;
      this->falseColor = falseColor;
      this->trueColor = trueColor;
      this->id = id;
    }
  };



  class AutonSystem {
    private:

      bool forceStop = false;

      bool loaded = false;
      bool runningSkills = false;
      bool runningFinals = false;

      double findNearestRot(double currentRot, double targetRot);
      inline double angleBetweenPoints(odom::Position pos1, odom::Position pos2);
      inline double distBetweenPoints(odom::Position pos1, odom::Position pos2);

      odom::Position target;
      odom::TilePosition startPos;

    public:

      odom::OdometrySystem* odometrySystemPointer;
      aiQueueSystem* queueSystemPtr;

      bool running = false;
      int teamColor = 0;

      std::string configFoler = "autonConfig/";
      std::string configFileType = ".txt";

      std::vector<autonConfig> configStorage;
      bool configMenuStatus = false;

      AutonSystem(odom::OdometrySystem* odometrySystemPointer, aiQueueSystem* queuePtr);

      void init();
      void generatePath();

      bool getConfig(const char* configId);
      void saveConfig(const char* configId, bool value);

      bool isReady();
      bool isRunningSkills();
      bool isFinals();

      void setStartPos();
      void setStartPos(odom::TilePosition pos);
      odom::Position getStartPos();
      odom::Position getTargetPos();

      inline void setForceStop(bool forceStop) { DEBUGLOG("Setting Force Stop: ", forceStop ? "true" : "false"); this->forceStop = forceStop; if (!forceStop) { running = false;} };
      inline bool getForceStop() { return this->forceStop; };

      bool driveDist(double dist);

      bool turnTo(double deg);
      bool turnTo(double deg, double turnTimeout);
      
      bool gotoLoc(odom::TilePosition pos);
      bool gotoLoc(odom::Position pos);
      
      bool longGoto(std::vector<odom::TilePosition> pos);
      bool longGoto(std::vector<odom::Position> pos);

      bool reverseDrive(double distance);

      bool catapult(int launchTime);
  };

  class aiQueueSystem {
    private:
      AutonSystem* aiPtr;
      odom::OdometrySystem* odomPtr;
      std::vector<autonMovement> queue;
      bool running = false;
      bool runMovement(autonMovement movement);

    public:

      bool loaded = false;

      void addPtrs(AutonSystem* botAIPtr, odom::OdometrySystem* odometryPointer);

      void runQueue();
      void clear();

      std::vector<autonMovement> getQueue();

      bool addToQueue(autonPath path);
      bool addToQueue(autonMovement movement);
      bool addToQueue(std::string jsonPath);

      autonPath getPathFromJSON(std::string jsonPath);

      void autonStarted();
  };

}