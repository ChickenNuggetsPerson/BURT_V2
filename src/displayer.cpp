
#include "displayer.h"
#include "robotConfig.h"
#include "displayApi.h"
#include "visionSensorConfig.h"
#include "mainController.h"

#include "string"

#include <iostream>
#include <fstream>
#include <vector>

#include <stdio.h>

using namespace vex;
using namespace display;


// Define head elements
Logger BrainLogs(1, 1, "logs.txt", 13, fontType::mono15);
MenuSystem mainRenderer(true, 600);


// Called when the screen is pressed
void screenPressed() {mainRenderer.screenPressed();}

// Define Notification Checks
bool checkSDCard() {          return Brain.SDcard.isInserted();                         }
bool checkMainController() {  return mainController.installed();                        }
bool checkFeild() {           return Competition.isFieldControl();                      }
bool checkBattery() {         return !(Brain.Battery.capacity(percentUnits::pct) < 20); }

int notificationCheck() {

    int checkSpeed = 1;

    NotificationChecker NotChecker(&mainRenderer);

    NotChecker.addMotor("RightMotorA", &rightMotorA);
    NotChecker.addMotor("RightMotorB", &rightMotorB);
    NotChecker.addMotor("RightMotorC", &rightMotorC);
    NotChecker.addMotor("LeftMotorA", &leftMotorA);
    NotChecker.addMotor("LeftMotorB", &leftMotorB);
    NotChecker.addMotor("LeftMotorC", &leftMotorC);
    
    NotChecker.addMotor("Flywheel", &catapultMotor);

    NotChecker.addCheck("SD Card Inserted", "SD Card Removed", checkSDCard, true);
    NotChecker.addCheck("Controller Connected", "Controller Disconnected", checkMainController, false, green, red, true);
    NotChecker.addCheck("Connected To Feild", "Feild Disconnect", checkFeild, false, purple, red, true);
    NotChecker.addCheck("", "Battery Low", checkBattery, true, color::white, color::red, true);

    while (true) {
        NotChecker.check();
        wait(checkSpeed, seconds);
    }

    return 1;
};


// Define the update function for the home page
int updateHome(Page* self) {
    self->setProgressBarValue("battery", Brain.Battery.capacity());
    self->setLineGraphValue("batWatt", Brain.Battery.current(currentUnits::amp) * Brain.Battery.voltage(voltageUnits::volt));

    if ( botAI.isRunningSkills()) {
        self->setTextData("skillsStatus", color::green, "[ Running Skills ]");
    } else {
        if (!botAI.getConfig("startSide")) {
            self->setTextData("skillsStatus", color::yellow, "[ Running Left ]");
        } else {
            self->setTextData("skillsStatus", color::yellow, "[ Running Right ]");
        }
    }
    self->setTextData("finalsStatus", color::green, botAI.isFinals() ? "Finals" : " ");
    
    if (Brain.Battery.capacity() > 50) {
        self->setTextData("batStatus", color::green, "OK");
    } else {
        if (Brain.Battery.capacity() > 20) {
            self->setTextData("batStatus", color::orange, "low");
        } else {
            self->setTextData("batStatus", color::red, "LOW");
        }
    }
    return 1;
}


// Define Standard Buttons
int gotoPrevPageButton(Page* self) {
    self->menuSystemPointer->gotoPrevPage();
    return 1;
}
int gotoMainPageButton(Page* self) {
    self->menuSystemPointer->gotoPage("main");
    return 1;
};
int gotoDebugPageButton(Page* self) {
    self->menuSystemPointer->gotoPage("debug");
    return 1;
};
int gotoConfigPageButton(Page* self) {
    self->menuSystemPointer->gotoPage("config");
    return 1;
};
int gotoQueuePageButton(Page* self) {
    self->menuSystemPointer->gotoPage("queue");
    return 1;
}
int gotoMapPageButton(Page* self) {
    self->menuSystemPointer->gotoPage("map");
    return 1;
}
int gotoSystemConfigButton(Page* self) {
    self->menuSystemPointer->gotoPage("systemConfig");
    return 1;
}
int gotoAutonPageButton(Page* self) {
    self->menuSystemPointer->gotoPage("auton");
    return 1;
}
int gotoDevicesDebugPageButton(Page* self) {
    self->menuSystemPointer->gotoPage("deviceDebug");
    return 1;
}

// Config Page Functions
void configPageInit(Page* currentPage, auton::AutonSystem* robotAI) {
    int x = 0;
    int y = 0;
    for (auto config: botAI.configStorage) {
        if (config.twoVals) {
            currentPage->addToggle(config.id.c_str(), config.name.c_str(), false, config.falseColor, config.trueColor, 20 + (110 * x), 80 + (60 * y), 100, 40, config.trueName.c_str());
        } else {
            currentPage->addToggle(config.id.c_str(), config.name.c_str(), false, config.falseColor, config.trueColor, 20 + (110 * x), 80 + (60 * y), 100, 40);
        }
        
        x++;
        if (x == 4) { y++; x = 0; }
    }
};
void setConfigs() {
    Page* autonConfigPage = mainRenderer.searchPages("config");
    for (auto config: botAI.configStorage) {
        autonConfigPage->setToggleStatus(config.id.c_str(), botAI.getConfig(config.id.c_str()));
        wait(0.01, seconds);
    }
};
void saveConfigs() {

    brainFancyDebug("Saving New Config", color::purple, true);
    Page* autonConfigPage = mainRenderer.searchPages("config");

    for (auto config: botAI.configStorage) {
        botAI.saveConfig(config.id.c_str(), autonConfigPage->getToggleStatus(config.id.c_str()));
    }
};

// Define the page loaded callback for the config page
int loadedConfigPage(Page* self) {
    self->pageChanged = false;
    if (!Brain.SDcard.isInserted()) {
        self->setTextData("savedStatus", red, "SD Card Not Inserted");
        botAI.configMenuStatus = false;
    } else {
        self->setTextData("savedStatus", color::white, "Loading Config...");
        botAI.configMenuStatus = true;
        wait(0.25, seconds);
        setConfigs();
        self->setTextData("savedStatus", color::white, "Ready");
    };
    return 1;
};
// Config Exit Button
int configExitButton(Page* self) {

    if (!botAI.configMenuStatus) {
        
        OverlayQuestion confirmOverlay;
        confirmOverlay.question = "What do you want to do?";
        confirmOverlay.option1 = "Reload";
        confirmOverlay.option1Color = green;
        confirmOverlay.option2 = "Exit";
        confirmOverlay.option2Color = red;

        if (self->overlayQuestion(confirmOverlay)) {
            // Option 2
            self->menuSystemPointer->gotoPage("main");
        } else {
            // Option 1
            self->menuSystemPointer->gotoPage("main");
            self->menuSystemPointer->gotoPage("config");
        }
    } else {
        if (self->pageChanged) {
            OverlayQuestion confirmOverlay;
            confirmOverlay.question = "Do You Want to Save?";
            confirmOverlay.option1 = "No";
            confirmOverlay.option1Color = red;
            confirmOverlay.option2 = "Yes";
            confirmOverlay.option2Color = green;

            if (self->overlayQuestion(confirmOverlay)) {
                // Option 2
                saveConfigs();
                botAI.init();

                self->menuSystemPointer->gotoPage("main");
            } else {
                // Option 1
                setConfigs();
                self->menuSystemPointer->gotoPage("main");
            }
        } else {
            self->menuSystemPointer->gotoPage("main");
        }
    }

    return 1;
};


int updateLoadingPage(Page* self) {

    double loadTime = 2;

    double stopTime = (loadTime * 1000);
    double percent = Brain.timer(timeUnits::msec) / stopTime;
    self->setProgressBarValue("load", (-(cos(PI*percent)-1)/2) * 100);

    if (percent > 1) { 
        self->menuSystemPointer->gotoPage("main"); 
        self->stopUpdater();
    }

    return 1;
}


void systemConfigSave(Page* self) {
    misc::writeFile(std::string(systemConfigFolder + systemArchivePath).c_str(), self->getToggleStatus("archiveLogs"));
    misc::writeFile(std::string(systemConfigFolder + systemDriveModePath).c_str(), self->getToggleStatus("arcadeDrive"));
    misc::writeFile(std::string(systemConfigFolder + systemEnableWSDebug).c_str(), self->getToggleStatus("enableWSDebug"));

}
int loadedSystemConfigPage(Page* self) {

    if (!Brain.SDcard.isInserted()) {
        self->setTextData("savedStatus", red, "SD Card Not Inserted");
        return 1;
    } 
    self->setTextData("savedStatus", color::white, "Edit System Config Values");

    // Set Archive Toggle
    self->setToggleStatus("archiveLogs", (misc::readFile(std::string(systemConfigFolder + systemArchivePath).c_str()) == 1));
    self->setToggleStatus("arcadeDrive", (misc::readFile(std::string(systemConfigFolder + systemDriveModePath).c_str()) == 1));
    self->setToggleStatus("enableWSDebug", (misc::readFile(std::string(systemConfigFolder + systemEnableWSDebug).c_str()) == 1));

    return 1;
}
int updateSystemConfig(Page* self) {

    return 1;
}
int systemConfigExitButton(Page* self) {

    if (!Brain.SDcard.isInserted()) {
        
        OverlayQuestion confirmOverlay;
        confirmOverlay.question = "What do you want to do?";
        confirmOverlay.option1 = "Reload";
        confirmOverlay.option1Color = green;
        confirmOverlay.option2 = "Exit";
        confirmOverlay.option2Color = red;

        if (self->overlayQuestion(confirmOverlay)) {
            // Option 2
            self->menuSystemPointer->gotoPage("main");
        } else {
            // Option 1
            self->menuSystemPointer->gotoPage("main");
            self->menuSystemPointer->gotoPage("systemConfig");
        }
    } else {
        if (self->pageChanged) {
            OverlayQuestion confirmOverlay;
            confirmOverlay.question = "Do You Want to Save?";
            confirmOverlay.option1 = "No";
            confirmOverlay.option1Color = red;
            confirmOverlay.option2 = "Yes";
            confirmOverlay.option2Color = green;

            if (self->overlayQuestion(confirmOverlay)) {
                // Option 2

                systemConfigSave(self);
                
                self->menuSystemPointer->newNotification("Restart Program", 10, color::white);
                self->menuSystemPointer->gotoPage("main");
            } else {
                // Option 1
                self->menuSystemPointer->gotoPage("main");
            }
        } else {
            self->menuSystemPointer->gotoPage("main");
        }
    }

    return 1;

}



// Define the update function for the debug page
int updateDebug(Page* self) {
    self->setProgressBarValue("fl", leftMotorA.temperature(percent));
    self->setProgressBarValue("fr", rightMotorA.temperature(percent));
    self->setProgressBarValue("bl", leftMotorB.temperature(percent));
    self->setProgressBarValue("br", rightMotorB.temperature(percent));

    if (Brain.SDcard.isInserted()) { // Update the SD Card Status Box
        self->setTextData("sdStatus", green);
    } else {
        self->setTextData("sdStatus", red);
    }
    
    if (Odometry.isTracking) { // Update the tracking status box
        self->setTextData("trackingStatus", green);
    } else {
        self->setTextData("trackingStatus", red);
    }

    if (botAI.isReady()) { // Update the auton status
        self->setTextData("autonStatus", green);
    } else {
        self->setTextData("autonStatus", red);
    }

    return 1;
}
// Declare Debug Menu Stuff
int dubugReloadButton(Page* self) {
    
    OverlayQuestion overlay;
    overlay.question = "Reload What?";
    overlay.option1 = "Auton";
    overlay.option1Color = cyan;
    overlay.option2 = "Odometry";
    overlay.option2Color = green;

    if (self->overlayQuestion(overlay)) {
        OverlayQuestion secondOverlay;
        secondOverlay.question = "Position";
        secondOverlay.option1 = "(0,0)";
        secondOverlay.option1Color = green;
        secondOverlay.option2 = "Match";
        if (Brain.SDcard.isInserted()) {
            secondOverlay.option2Color = green;
        } else {
            secondOverlay.option2Color = red;
        }
        wait(0.5, timeUnits::sec);
        if (self->overlayQuestion(secondOverlay)) {
            Odometry.restart(botAI.getStartPos());
        } else {
            Odometry.restart();
        }
        
    } else {
        botAI.init();
    }

    return 1;
};


// Define the update function for the odometry page
int updateQueuePage(Page* self) {
    Plot* plotPtr = self->getPlotPointer("map");
    auton::autonPath path = auton::autonPath();
    path.startPos = botAI.getStartPos();
    
    std::vector<auton::autonMovement> movements = queuingSystem.getQueue();
    for (auto movement: movements) {
        path.add(movement);
    }
    self->setTextData("size", (int)movements.size());
    plotPtr->showPath(path);
    return 1;
};
int regenerateQueueButton(Page* self) {
    botAI.generatePath();
    return 1;
};
int resetQueueButton(Page* self) {
    queuingSystem.clear();
    return 1;
};

// Update the Map Page
int updateMap(Page* self) {
    if (Odometry.isTracking) {
        self->setTextData("status", green, "Running");
    } else {
        self->setTextData("status", red, "Odom Not Running");
        return 1;
    }

    Plot* plotPtr = self->getPlotPointer("map"); 
    odom::Position currentPos = Odometry.currentPos();
    currentPos.rot = misc::radToDegree(currentPos.rot);

    plotPtr->point1 = currentPos;
    plotPtr->drawPoint1 = true;

    if ( botAI.running ) {
        plotPtr->updatePoint(2, true, botAI.getTargetPos());
        plotPtr->updateLine(true);
    } else {
        plotPtr->updatePoint(2, false);
        plotPtr->updateLine(false);
    }

    self->setTextData("xpos", currentPos.x);
    self->setTextData("ypos", currentPos.y);
    self->setTextData("rot", (currentPos.rot));

    return 1;
}
int mapShowPath(Page* self) {
    Plot* plotPtr = self->getPlotPointer("map");
    if (self->pageChanged) {
        self->pageChanged = false;
        plotPtr->drawingPath = false;
        self->setButtonData("showPathBtn", color::black, "Show Path");
        return 1;
    }

    self->setButtonData("showPathBtn", color::black, "Clear Path");

    OverlayQuestion overlay;
    overlay.question = "Overlay?";
    overlay.option1 = "Queue";
    overlay.option1Color = color::cyan;
    overlay.option2 = "JSON";
    overlay.option2Color = color::green;

    if (!self->overlayQuestion(overlay)) {

        auton::autonPath path = auton::autonPath();
        path.startPos = botAI.getStartPos();
        
        std::vector<auton::autonMovement> movements = queuingSystem.getQueue();
        for (auto movement: movements) {
            path.add(movement);
        }

        plotPtr->showPath(path);
        self->pageChanged = true;
    } else {
        std::vector<const char *> pathList;
        pathList.push_back("right.json");
        pathList.push_back("left.json");
        pathList.push_back("skills.json");
        pathList.push_back("path.json");

        int result = mainControllerPickOption(pathList);
        plotPtr->showPath(queuingSystem.getPathFromJSON(std::string(AUTON_PATH_FOLDER + pathList.at(result))));
        self->pageChanged = true;

    }
    

    return 1;
}


void addDrivetrain(Page* self, int x, int y) {
    int width = 85;
    int height = 110;

    self->addDisplayBox("", x, y, width, height, color::black);
    self->addDisplayBox("", x + 5, y + 5, width - 10, height - 10, color::black);

    self->addDisplayBox("lmA", x, y + 10, 20, 20, color::red);
    self->addDisplayBox("lmB", x, y + 35, 20, 20, color::red);
    self->addDisplayBox("lmC", x, y + 70, 20, 20, color::red);

    self->addDisplayBox("rmA", x + 65, y + 10, 20, 20, color::red);
    self->addDisplayBox("rmB", x + 65, y + 35, 20, 20, color::red);
    self->addDisplayBox("rmC", x + 65, y + 70, 20, 20, color::red);


    // x = 110
    // y = 60

    self->addLine(Line(x - 30, y + 10, x - 10, y + 20));
    self->addLine(Line(x - 60, y + 10, x - 30, y + 10));
    self->addText("%d C", x - 60, y + 5, color::white, fontType::mono15, "lmA");

    self->addLine(Line(x - 50, y + 35, x - 10, y + 45));
    self->addLine(Line(x - 80, y + 35, x - 50, y + 35));
    self->addText("%d C", x - 80, y + 30, color::white, fontType::mono15, "lmB");

    self->addLine(Line(x - 30, y + 90, x - 10, y + 80));
    self->addLine(Line(x - 60, y + 90, x - 30, y + 90));
    self->addText("%d C", x - 60, y + 85, color::white, fontType::mono15, "lmC");


    self->addLine(Line(x + 115, y + 10, x + 100, y + 20));
    self->addLine(Line(x + 145, y + 10, x + 115, y + 10));
    self->addText("%d C", x + 120, y + 5, color::white, fontType::mono15, "rmA");

    self->addLine(Line(x + 135, y + 35, x + 100, y + 45));
    self->addLine(Line(x + 165, y + 35, x + 135, y + 35));
    self->addText("%d C", x + 140, y + 30, color::white, fontType::mono15, "rmB");

    self->addLine(Line(x + 115, y + 90, x + 100, y + 80));
    self->addLine(Line(x + 145, y + 90, x + 115, y + 90));
    self->addText("%d C", x + 120, y + 85, color::white, fontType::mono15, "rmC");
}
void addSideView(Page* self, int x, int y) {

    
    self->addDisplayBox("", x + 5, y + 30, 10, 30, color::black); // Brain

    // Wheels
    self->addDisplayBox("", x + 5, y + 50, 20, 20, vex::color(200, 200, 200));
    self->addDisplayBox("", x + 30, y + 50, 20, 20, vex::color(200, 200, 200));
    self->addDisplayBox("", x + 55, y + 50, 20, 20, vex::color(200, 200, 200));
    self->addDisplayBox("", x + 80, y + 50, 20, 20, vex::color(200, 200, 200));

    // Flywheel
    self->addDisplayBox("", x + 38, y + 20, 20, 40, color::black);
    self->addCircle(Circle(x + 48, y + 10, 15, color(230, 230, 230)));
    self->addDisplayBox("flywheel", x + 40, y, 15, 30, color::black);
    
    // Drivetrain
    self->addDisplayBox("", x, y + 50, 110, 15, color::black);

    // Add intake
    self->addLine(Line(x + 60, y + 50, x + 120, y + 5));
    self->addLine(Line(x + 80, y + 50, x + 120, y + 20));
    self->addLine(Line(x + 120, y + 5, x + 120, y + 20));

    self->addCircle(Circle(x + 128, y + 14, 19, color(230, 230, 230)));
    self->addDisplayBox("intake", x + 120, y, 15, 30, color::black);


    self->addText("Flywheel: %d C", x - 120, y + 20, color::white, fontType::mono15, "flywheel");
    self->addText("Intake: %d C", x - 120, y + 40, color::white, fontType::mono15, "intake");
}

void buildDeviceDebugPage(Page* self) {
    
    // self->addLine(Line(100, 100, 400, 200, 5, vex::color::blue));

    // Make Bot Visualization
    addDrivetrain(self, 90, 55);
    addSideView(self, 190, 170);

    self->addText("Feild Status", 340, 60, white, fontType::mono20, "filed");
    self->addText("Odom System", 340, 80, white, fontType::mono20, "odom");
    self->addText("Queue System", 340, 100, white, fontType::mono20, "queue");
    self->addText("Auton System", 340, 120, white, fontType::mono20, "auton");
    self->addText("SD Card", 340, 140, white, fontType::mono20, "sd");
}
void updateMotorBox(Page* self, const char* id, vex::motor* mtr) {

    if (!mtr->installed()) {
        self->setTextData(id, 0);
        self->setDisplayBoxData(id, color::black);
        return;
    }

    self->setTextData(id, (int)mtr->temperature(temperatureUnits::celsius));

    if (mtr->temperature(temperatureUnits::celsius) < 40) {
        self->setDisplayBoxData(id, color::green);
    } else {
        if (mtr->temperature(temperatureUnits::celsius) > 50) {
            self->setDisplayBoxData(id, color::red);
        } else {
            self->setDisplayBoxData(id, color::orange);
        }
    }
}

int updateDeviceDebug(Page* self) {
    
    // Set Motor temps and colors
    updateMotorBox(self, "lmA", &leftMotorA);
    updateMotorBox(self, "lmB", &leftMotorB);
    updateMotorBox(self, "lmC", &leftMotorC);

    updateMotorBox(self, "rmA", &rightMotorA);
    updateMotorBox(self, "rmB", &rightMotorB);
    updateMotorBox(self, "rmC", &rightMotorC);

    updateMotorBox(self, "flywheel", &catapultMotor);
    updateMotorBox(self, "intake", &intakeMotor);


    self->setTextData("filed", Competition.isFieldControl() ? green : red);
    if (Odometry.isTracking) {
        if (Odometry.usingDrive) {
            self->setTextData("odom", yellow);
        } else {
            self->setTextData("odom", green);
        }
    } else {
        self->setTextData("odom", red);
    }
    self->setTextData("queue", queuingSystem.loaded ? green : red);
    self->setTextData("auton", botAI.isReady() ? green : red);
    self->setTextData("sd", Brain.SDcard.isInserted() ? green : red);
    return 1;
}


// Initialize All The Pages
int brainDisplayerInit() {

    Brain.Screen.pressed(screenPressed);
    task notificationTask(notificationCheck, task::taskPrioritylow);

    // Init Gradients
    Gradient batteryGradient = Gradient(1, 100, 15, 70);
    Gradient heatGradient = Gradient(100, 1, 60, 80);

    Gradient loadingGradient;
    if (Competition.isFieldControl()) {
        loadingGradient = Gradient(0, 360, 0, 100);
    } else {
        loadingGradient.finalGradient = {colorRange(-200, 200, color::white)};
    }

    // Define Pages
    Page loadingPage;
    Page homePage;
    Page mapPage;
    Page debugPage;
    Page queuePage;
    Page autonConfigPage;
    Page systemConfigPage;
    Page devicesDebug;

    // Configure the loading page
    loadingPage.addText("BURT OS", 140, 100, color::white, fontType::mono60);
    loadingPage.addText("Developed by Hayden Steele", 140, 130, color::white, fontType::mono15);
    loadingPage.addHorzProgressBar("load", 140, 150, 210, 20, " ", false, false, loadingGradient.finalGradient);

    loadingPage.addDataUpdaterCB(updateLoadingPage, 0.01);

    // Configure the home page
    homePage.addText("BURT OS", 20, 50, color::white, fontType::mono40);
    homePage.addText("Developed by Hayden Steele", 22, 75, color::white, fontType::mono15);
    homePage.addText("Skills Status", 22, 110, color::white, fontType::mono20, "skillsStatus");
    homePage.addText("Skills Status", 22, 130, color::white, fontType::mono20, "finalsStatus");
    homePage.addButton("Debug", 380, 210, 100, 30, gotoDebugPageButton, "debugPageButton");
    homePage.addButton("Config", 280, 210, 100, 30, gotoConfigPageButton, "configPageButton");
    homePage.addButton("Map", 180, 210, 100, 30, gotoMapPageButton, "mapPageButton");
    homePage.addButton("Devies", 80, 210, 100, 30, gotoDevicesDebugPageButton, "systemButton");
    homePage.addHorzProgressBar("battery", 325, 15, 150, 30, "Battery: %d%%", false, false, batteryGradient.finalGradient);
    homePage.addText("Status: ", 325, 70, color::white, fontType::prop20);
    homePage.addText("YEET", 390, 70, color::white, fontType::prop20, "batStatus");
    homePage.addLineGraph("batWatt", "Watts: %dW", 325, 100, 150, 75, false, heatGradient.finalGradient, 100);
    homePage.addDataUpdaterCB(updateHome, 0.5);

    // Configure the map page
    mapPage.addText("Feild Map", 20, 40, color::white, fontType::mono30, "title");
    mapPage.addText("Status", 22, 65, color::white, fontType::mono15, "status");
    mapPage.addPlot("map", "Robot Pos", 175, 15, 200, 200, odom::tileWidth*6, odom::tileWidth*6, 6, true, TEAM_RED);
    mapPage.addText("X:   %f", 20, 100, color::white, fontType::mono20, "xpos");
    mapPage.addText("Y:   %f", 20, 130, color::white, fontType::mono20, "ypos");
    mapPage.addText("Rot: %f", 20, 160, color::white, fontType::mono20, "rot");
    mapPage.addButton("Back", 380, 210, 100, 30, gotoPrevPageButton, "prevPageButton");
    mapPage.addButton("Show Path", 380, 70, 100, 30, mapShowPath, "showPathBtn");
    mapPage.addDataUpdaterCB(updateMap, 0.2);


    // Configure the auton config page
    autonConfigPage.addText("Configure Auton", 20, 40, color::white, fontType::mono30, "title");
    autonConfigPage.addText("Status", 22, 65, color::white, fontType::mono15, "savedStatus");
    autonConfigPage.addButton("Back", 380, 210, 100, 30, configExitButton, "mainPageButton");
    configPageInit(&autonConfigPage, &botAI);
    autonConfigPage.addPageLoadedCB(loadedConfigPage);


    // Configure the system config page
    systemConfigPage.addText("Configure System", 20, 40, color::white, fontType::mono30, "title");
    systemConfigPage.addText("Status", 22, 65, color::white, fontType::mono15, "savedStatus");
    systemConfigPage.addButton("Back", 380, 210, 100, 30, systemConfigExitButton, "mainPageButton");
    systemConfigPage.addToggle("archiveLogs", "Archive Logs", false, vex::color(168, 0, 0), vex::color(0, 168, 0), 20, 170, 150, 40);
    systemConfigPage.addToggle("arcadeDrive", "Arcade Drive", true, vex::color(168, 0, 0), vex::color(0, 168, 0), 20, 120, 150, 40, "Tank Drive");
    systemConfigPage.addToggle("enableWSDebug", "WS Debug", true, vex::color(168, 0, 0), vex::color(0, 168, 0), 200, 120, 150, 40);

    systemConfigPage.addPageLoadedCB(loadedSystemConfigPage);
    systemConfigPage.addDataUpdaterCB(updateSystemConfig);

    
    // Configure the debug page
    debugPage.addLogger(&BrainLogs);
    debugPage.addHorzProgressBar("fl", 300, 15, 175, 15, "FL %d%%", false, false, heatGradient.finalGradient);
    debugPage.addHorzProgressBar("fr", 300, 53, 175, 15, "FR %d%%", false, false, heatGradient.finalGradient);
    debugPage.addHorzProgressBar("bl", 300, 91, 175, 15, "BL %d%%", false, false, heatGradient.finalGradient);
    debugPage.addHorzProgressBar("br", 300, 129, 175, 15, "BR %d%%", false, false, heatGradient.finalGradient);

    debugPage.addText("SD Card",  300, 165, color::white, fontType::mono20, "sdStatus");
    debugPage.addText("Odometry Status", 300, 185, color::white, fontType::mono20, "trackingStatus");
    debugPage.addText("Auton Status",    300, 205, color::white, fontType::mono20, "autonStatus");

    debugPage.addButton("Reload", 80, 210, 100, 30, dubugReloadButton, "reloadButton");
    debugPage.addButton("System", 180, 210, 100, 30, gotoSystemConfigButton, "systemButton");
    debugPage.addButton("Queue", 280, 210, 100, 30, gotoQueuePageButton, "odometryPageButton");
    debugPage.addButton("Back", 380, 210, 100, 30, gotoMainPageButton, "mainPageButton");
    debugPage.addDataUpdaterCB(updateDebug, 1);


    // Config the Odometry Page
    queuePage.addText("Auton Queue", 20, 40, color::white, fontType::mono30, "title");
    queuePage.addText("Queue Size: %d", 60, 100, color::white, fontType::mono20, "size");
    queuePage.addPlot("map", " ", 275, 2, 200, 200, odom::tileWidth*6, odom::tileWidth*6, 6, true, TEAM_RED);
    queuePage.addButton("Regenerate", 60, 120, 120, 30, regenerateQueueButton);
    queuePage.addButton("Clear", 60, 160, 120, 30, resetQueueButton);
    queuePage.addButton("Back", 380, 210, 100, 30, gotoDebugPageButton, "mainPageButton");
    queuePage.addDataUpdaterCB(updateQueuePage, 1);


    // Device Debug
    devicesDebug.addText("System Status: ", 20, 35, color::white, fontType::mono30, "title");
    devicesDebug.addText("Systems: ", 340, 35, color::white, fontType::mono30, "altTitle");

    buildDeviceDebugPage(&devicesDebug);

    devicesDebug.addButton("Back", 380, 210, 100, 30, gotoPrevPageButton, "prevPageButton");
    devicesDebug.addDataUpdaterCB(updateDeviceDebug, 1);

    // Add pages to the main renderer
    mainRenderer.addPage("loading", loadingPage);
    mainRenderer.addPage("main", homePage);
    mainRenderer.addPage("debug", debugPage);
    mainRenderer.addPage("config", autonConfigPage);
    mainRenderer.addPage("systemConfig", systemConfigPage);
    mainRenderer.addPage("queue", queuePage);
    mainRenderer.addPage("map", mapPage);
    mainRenderer.addPage("deviceDebug", devicesDebug);

    return 1;
};



#include "motionProfiling.h"

// Main Loop For Rendering the Brain Display
int brainDisplayer() {

    brainDisplayerInit();
    mainRenderer.ready();
    BrainLogs.init();

    double deltaTime = 0.00;
    while(true) {  // Main render loop
        double startTime = Brain.timer(msec);
        Brain.Screen.clearScreen();
        mainRenderer.render(); // Render the screen

        Brain.Screen.setFont(fontType::mono15);
        Brain.Screen.setFillColor(isWriting > 0 ? vex::color::red : vex::color::transparent);
        
        if (isWriting >= 0) {
            isWriting--;
        }

        Brain.Screen.drawRectangle(-1, 220, 75, 20);
        Brain.Screen.printAt(1, 235, "FPS: %d", int(deltaTime)); // Show the screen FPS
        deltaTime = 1000 / (round(Brain.timer(msec) - startTime)); // Calculate the fps
        
        Brain.Screen.render(); // Use the Screen.Render() to stop visual bugs
    }


    // motionProfiling::Profile result = motionProfiling::genVelProfile(24*5);

    // Brain.Screen.clearScreen();
    
    // int screenXSize = 480;
    // int screenYSize = 240;

    // Brain.Screen.setFillColor(color::transparent);
    // Brain.Screen.setPenColor(color::white);
    // Brain.Screen.setPenWidth(1);

    // Brain.Screen.drawRectangle(screenXSize * 0.1, screenYSize * 0.1, screenXSize * 0.8, screenYSize * 0.8);

    // Brain.Screen.setPenWidth(2);

    // double prevX = screenXSize * 0.1;
    // double prevY = screenYSize * 0.9;

    // double x = 0.00;
    // double y = 0.00;

    // double wStep = ((screenXSize * 0.8) / 10) * motionProfiling::timeIncrement;
    // for (int i = 0; i < result.get()->size(); i++) {
        
    //     double val = (screenYSize * 0.9) - result.get()->at(i);
    //     Brain.Screen.setPenColor(color::red);
    //     Brain.Screen.drawLine(prevX, prevY, (screenXSize * 0.1) + i * wStep, val);

    //     prevX = (screenXSize * 0.1) + i * wStep;
    //     prevY = val;


    //     double otherY = y;

    //     x += motionProfiling::timeIncrement;
    //     y += result.get()->at(i) * motionProfiling::timeIncrement;

    //     Brain.Screen.setPenColor(color::green);
    //     Brain.Screen.drawLine(
    //         (screenXSize * 0.1) + (i-1) * wStep, 
    //         (screenYSize * 0.9) - otherY, 
    //         (screenXSize * 0.1) + i * wStep, 
    //         (screenYSize * 0.9) - y);
    // }

    // DEBUGLOG("Final Y: ", y);

    return 1;
}








// Main Loop For Rendering the Controllers
int controllerDisplay() {
    while (true){

        // Main Controller Displayer
        if (mainController.installed()) {
            mainControllerRender();
        }

        // Alt Controller Displayer
        if (altController.installed()) {
            altControllerRender();
        }

        vex::wait(0.2, seconds);
    }
    return 1;
}











// Change the screen page
void brainChangePage(const char* pageName) {
    mainRenderer.gotoPage(pageName);
};

void brainPageChangeData(const char* pageName, const char* pointId, int data) {
    Page* pagePointer = mainRenderer.searchPages(pageName);
    if (pagePointer == nullptr) { return; }
    pagePointer->massSetData(pointId, data);
}

void brainPageChangeText(const char* pageName, const char* textId, vex::color displayColor) {
    Page* pagePointer = mainRenderer.searchPages(pageName);
    if (pagePointer == nullptr) { return; }
    pagePointer->setTextData(textId, displayColor);
}
void brainPageChangeText(const char* pageName, const char* textId, const char* newText) {
    Page* pagePointer = mainRenderer.searchPages(pageName);
    if (pagePointer == nullptr) { return; }
    pagePointer->setTextData(textId, NAN, newText);
}
void brainPageChangeText(const char* pageName, const char* textId, int data) {
    Page* pagePointer = mainRenderer.searchPages(pageName);
    if (pagePointer == nullptr) { return; }
    pagePointer->setTextData(textId, data);
}


// Displays error on brain screen
void brainError(const char* message) {
  
    BrainLogs.newLog(message, vex::color::red);
    mainRenderer.newNotification(message, 5, red);

    DEBUGLOG("ERROR: ", message);
}
void brainError(const char* message, bool showOnController) {
    BrainLogs.newLog(message, vex::color::red);
    mainRenderer.newNotification(message, 5, red);
    mainController.rumble("--");
    mainControllerMessage(message, 5);
    DEBUGLOG("ERROR: ", message);
}

// Displays debug message on brain screen
void brainDebug(const char* message) {
  
    BrainLogs.newLog(message, vex::color::purple);

    DEBUGLOG("DEBUG: ", message);
}
void brainDebug(const char* message, bool notification) {
  
    BrainLogs.newLog(message, vex::color::purple);
    mainRenderer.newNotification(message, 4, purple);

    DEBUGLOG("DEBUG: ", message);
}

// Displays debug message on brain screen
void brainFancyDebug(const char* message, vex::color messageColor) {
  
    BrainLogs.newLog(message, messageColor);

    DEBUGLOG("DEBUG: ", message);
}

// Displays debug message on brain screen
// Has the ability to also show number info with the message
void brainFancyDebug(const char* message, vex::color messageColor, int data) {
  
    BrainLogs.newLog(message, messageColor, data);

    DEBUGLOG("DEBUG: ", message);
}
void brainFancyDebug(const char* message, vex::color messageColor, bool showNotification) {
  
    BrainLogs.newLog(message, messageColor);
    mainRenderer.newNotification(message, 4, messageColor);

    DEBUGLOG("DEBUG: ", message);
}