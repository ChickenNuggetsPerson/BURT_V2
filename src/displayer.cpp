
#include "displayer.h"
#include "robotConfig.h"
#include "displayApi.h"


#include "string"

#include <iostream>
#include <fstream>
#include <vector>

#include <stdio.h>

using std::cout;
using std::endl;

using namespace vex;



// Define head elements
Logger BrainLogs(1, 1, "logs.txt", 10);
MenuSystem mainRenderer(true);

// Define Pages
Page homePage;
Page mapPage;
Page debugPage;
Page odometryPage;
Page autonConfigPage;
Page systemConfigPage;

Page testPage;


// Called when the screen is pressed
void screenPressed() {mainRenderer.screenPressed();}

// Define Notification Checks
bool checkInertial() {return inertialSensor.isCalibrating();}
bool checkSDCard() {return Brain.SDcard.isInserted();}
bool checkMainController() {return mainController.installed();}
bool checkFeild() {return Competition.isFieldControl();}

int notificationCheck() {
    
    int checkSpeed = 1;

    NotificationChecker NotChecker(&mainRenderer);
    MotorChecker MotChecker(&mainRenderer);


    // Fix this, (Causes Memory Leak) 

    //MotChecker.addCheck(&leftMotorA, "LeftMotorA");
    //MotChecker.addCheck(&leftMotorB, "LeftMotorB");
    //MotChecker.addCheck(&rightMotorA, "RightMotorA");
    //MotChecker.addCheck(&rightMotorB, "RightMotorB");

    NotChecker.addCheck("Starting Calibration", "Done Calibrating", checkInertial, true, yellow, orange, true);
    NotChecker.addCheck("SD Card Inserted", "SD Card Removed", checkSDCard, true);
    NotChecker.addCheck("Controller Connected", "Controller Disconnected", checkMainController, false, green, red, true);
    NotChecker.addCheck("Connected To Feild", "Feild Disconnect", checkFeild, false, purple, red, true);

    while (true) {
        NotChecker.check();
        MotChecker.check();

        wait(checkSpeed, seconds);
    }

    return 1;
};


// Define the update function for the home page
int updateHome(Page* self) {
    self->setProgressBarValue("battery", Brain.Battery.capacity());
    
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
int gotoOdometryPageButton(Page* self) {
    self->menuSystemPointer->gotoPage("odometry");
    return 1;
}
int gotoMapPageButton(Page* self) {
    self->menuSystemPointer->gotoPage("map");
    return 1;
}
int gotoTestPageButton(Page* self) {
    self->menuSystemPointer->gotoPage("test");
    return 1;
}
int gotoSystemConfigButton(Page* self) {
    self->menuSystemPointer->gotoPage("systemConfig");
    return 1;
}

// Config Page Functions
void configPageInit(Page* currentPage, ai* robotAI) {
    int x = 0;
    int y = 0;
    for (int i = 0; i < robotAI->totalConfigs; i++) {
        currentPage->addToggle(robotAI->configNames[i].c_str(), false, vex::color(168, 0, 0), vex::color(0, 168, 0), 20 + (110 * x), 80 + (60 * y), 100, 40);
        x++;
        if (x == 4) { y++; x = 0; }
    }
};
void setConfigs() {
    for (int i = 0; i < botAI.totalConfigs; i++) {
        autonConfigPage.setToggleStatus(botAI.configNames[i].c_str(), botAI.getConfig(botAI.configNames[i].c_str()));
        wait(0.01, seconds);
    }
};
void saveConfigs() {

    brainFancyDebug("Saving New Config", color::purple, true);

    for (int i = 0; i < botAI.totalConfigs; i++) {
        botAI.saveConfig(botAI.configNames[i].c_str(), autonConfigPage.getToggleStatus(botAI.configNames[i].c_str()));
    }
};

// Define the page loaded callback for the config page
int loadedConfigPage(Page* self) {
    self->pageChanged = false;
    if (!Brain.SDcard.isInserted()) {
        self->setTextData("savedStatus", red, "SD Card Not Inserted");
        botAI.configMenuStatus = false;
    } else {
        self->setTextData("savedStatus", white, "Loading Config...");
        botAI.configMenuStatus = true;
        wait(0.25, seconds);
        setConfigs();
        self->setTextData("savedStatus", white, "Ready");
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


void systemConfigSave(Page* self) {
    writeFile(std::string(systemConfigFolder + systemArchivePath).c_str(), self->getToggleStatus("Archive Logs"));


}
int loadedSystemConfigPage(Page* self) {

    if (!Brain.SDcard.isInserted()) {
        self->setTextData("savedStatus", red, "SD Card Not Inserted");
        return 1;
    } 
    self->setTextData("savedStatus", white, "Edit System Config Values");

    // Set Archive Toggle
    if (readFile(std::string(systemConfigFolder + systemArchivePath).c_str()) == 1) {
        self->setToggleStatus("Archive Logs", true);
    } else {
        self->setToggleStatus("Archive Logs", false);
    }

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
                
                self->menuSystemPointer->newNotification("Restart Program", 10, white);
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

    if (Competition.isFieldControl()) { // Update feild stats
        self->setTextData("feildStatus", green);
    } else {
        self->setTextData("feildStatus", red);
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
        wait(0.5, sec);
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
int updateOdometry(Page* self) {
    if (Odometry.isTracking) {
        self->setTextData("status", green, "Running");
    } else {
        self->setTextData("status", red, "Not Running");
    }

    Position currentPos = Odometry.currentPos();

    self->setTextData("xpos", currentPos.x);
    self->setTextData("ypos", currentPos.y);
    self->setTextData("rot", (radToDegree(currentPos.rot)));

    TilePosition currentTile = Odometry.currentTilePos();

    self->setTextData("xtile", currentTile.x);
    self->setTextData("ytile", currentTile.y);

    return 1;
};


int updateMap(Page* self) {
    if (Odometry.isTracking) {
        self->setTextData("status", green, "Running");
    } else {
        self->setTextData("status", red, "Odom Not Running");
        return 1;
    }

    Plot* plotPtr = self->getPlotPointer("map"); 
    Position currentPos = Odometry.currentPos();
    currentPos.rot = radToDegree(currentPos.rot);

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




int loadedTestPage(Page* self) {

    return 1;
}
int testPageUpdater(Page* self) {

    return 1;
}


int brainDisplayerInit() {


    Brain.Screen.pressed(screenPressed);
    task notificationTask(notificationCheck, task::taskPrioritylow);

    // Init Gradients
    Gradient batteryGradient = Gradient(1, 100, 15, 70);
    Gradient heatGradient = Gradient(100, 1, 60, 80);
    Gradient graphGradient = Gradient(10, 300, 10, 100);

    // Add pages to the main renderer
    mainRenderer.addPage("main", &homePage);
    mainRenderer.addPage("debug", &debugPage);
    mainRenderer.addPage("config", &autonConfigPage);
    mainRenderer.addPage("systemConfig", &systemConfigPage);
    mainRenderer.addPage("odometry", &odometryPage);
    mainRenderer.addPage("map", &mapPage);
    mainRenderer.addPage("test", &testPage);


    testPage.addPageLoadedCB(loadedTestPage);
    testPage.addDataUpdaterCB(testPageUpdater);


    // Configure the home page
    homePage.addText("BURT OS", 20, 50, white, fontType::mono40);
    homePage.addText("Developed by Hayden Steele", 22, 75, white, fontType::mono15);
    homePage.addButton("Debug", 380, 210, 100, 30, gotoDebugPageButton, "debugPageButton");
    homePage.addButton("Config", 280, 210, 100, 30, gotoConfigPageButton, "configPageButton");
    homePage.addButton("Map", 180, 210, 100, 30, gotoMapPageButton, "mapPageButton");
    homePage.addHorzProgressBar("battery", 325, 15, 150, 30, "Battery: %d%%", false, batteryGradient.finalGradient);
    homePage.addDataUpdaterCB(updateHome, 1);

    homePage.addButton("test", 20, 150, 100, 30, gotoTestPageButton, "test");


    // Configure the map page
    mapPage.addText("Feild Map", 20, 40, white, fontType::mono30, "title");
    mapPage.addText("Status", 22, 65, white, fontType::mono15, "status");
    mapPage.addPlot("map", "Robot Pos", 175, 15, 200, 200, tileWidth*6, tileWidth*6, 6, true, TEAM_RED);
    mapPage.addText("X:   %f", 20, 100, white, fontType::mono20, "xpos");
    mapPage.addText("Y:   %f", 20, 130, white, fontType::mono20, "ypos");
    mapPage.addText("Rot: %f", 20, 160, white, fontType::mono20, "rot");
    mapPage.addButton("Back", 380, 210, 100, 30, gotoPrevPageButton, "prevPageButton");
    mapPage.addDataUpdaterCB(updateMap, 0.2);


    // Configure the auton config page
    autonConfigPage.addText("Configure Auton", 20, 40, white, fontType::mono30, "title");
    autonConfigPage.addText("Status", 22, 65, white, fontType::mono15, "savedStatus");
    autonConfigPage.addButton("Back", 380, 210, 100, 30, configExitButton, "mainPageButton");
    configPageInit(&autonConfigPage, &botAI);
    autonConfigPage.addPageLoadedCB(loadedConfigPage);


    // Configure the system config page
    systemConfigPage.addText("Configure System", 20, 40, white, fontType::mono30, "title");
    systemConfigPage.addText("Status", 22, 65, white, fontType::mono15, "savedStatus");
    systemConfigPage.addButton("Back", 380, 210, 100, 30, systemConfigExitButton, "mainPageButton");

    systemConfigPage.addToggle("Archive Logs", false, vex::color(168, 0, 0), vex::color(0, 168, 0), 20, 170, 150, 40);
    
    systemConfigPage.addPageLoadedCB(loadedSystemConfigPage);
    systemConfigPage.addDataUpdaterCB(updateSystemConfig);

    
    // Configure the debug page
    debugPage.addLogger(&BrainLogs);
    debugPage.addVertProgressBar("fl", 300, 15, 30, 100, "%d%%", false, heatGradient.finalGradient);
    debugPage.addVertProgressBar("fr", 350, 15, 30, 100, "%d%%", false, heatGradient.finalGradient);
    debugPage.addVertProgressBar("bl", 400, 15, 30, 100, "%d%%", false, heatGradient.finalGradient);
    debugPage.addVertProgressBar("br", 450, 15, 30, 100, "%d%%", false, heatGradient.finalGradient);

    debugPage.addText("SD Card",  300, 140, white, fontType::mono20, "sdStatus");
    debugPage.addText("Odometry Status", 300, 160, white, fontType::mono20, "trackingStatus");
    debugPage.addText("Auton Status",    300, 180, white, fontType::mono20, "autonStatus");
    debugPage.addText("Connected to Feild",    300, 200, white, fontType::mono20, "feildStatus");

    debugPage.addButton("Reload", 80, 210, 100, 30, dubugReloadButton, "reloadButton");
    debugPage.addButton("System", 180, 210, 100, 30, gotoSystemConfigButton, "systemButton");
    debugPage.addButton("Odometry", 280, 210, 100, 30, gotoOdometryPageButton, "odometryPageButton");
    debugPage.addButton("Back", 380, 210, 100, 30, gotoMainPageButton, "mainPageButton");
    debugPage.addDataUpdaterCB(updateDebug, 1);


    // Config the Odometry Page
    odometryPage.addText("Odometry Debug", 20, 40, white, fontType::mono30, "title");
    odometryPage.addText("Status", 22, 65, white, fontType::mono15, "status");
    odometryPage.addText("X:   %f", 60, 100, white, fontType::mono30, "xpos");
    odometryPage.addText("Y:   %f", 60, 140, white, fontType::mono30, "ypos");
    odometryPage.addText("Rot: %f", 60, 180, white, fontType::mono30, "rot");
    odometryPage.addText("%f", 300, 100, white, fontType::mono30, "xtile");
    odometryPage.addText("%f", 300, 140, white, fontType::mono30, "ytile");
    odometryPage.addButton("Back", 380, 210, 100, 30, gotoDebugPageButton, "mainPageButton");
    odometryPage.addButton("Map", 280, 210, 100, 30, gotoMapPageButton, "mapPageButton");
    odometryPage.addDataUpdaterCB(updateOdometry, 0.05);

    return 1;
};





// Main Loop For Rendering the Brain Display
int brainDisplayer() {

    brainDisplayerInit();
    
    double deltaTime = 0.00;
    while(true) {
        double startTime = Brain.timer(msec);
        Brain.Screen.clearScreen();
        Brain.Screen.printAt(1, 235, "FPS: %d", int(deltaTime)); // Show the screen FPS
        mainRenderer.render(); // Render the screen
        deltaTime = 1000 / (round(Brain.timer(msec) - startTime)); // Calculate the fps
        Brain.Screen.render(); // Use the Screen.Render() to stop visual bugs
    }

    return 1;
}

 






 
// Main Loop For Rendering the Controllers
int controllerDisplay() {
    while (true){
        
        if (mainController.installed()) {
            // Main Controller Displayer
            mainControllerRender();
        }

        // Alt Controller Displayer
        if (altController.installed()) {
            altController.Screen.clearScreen();
            altController.Screen.setCursor(1, 1);
            altController.Screen.print("Alt Controller");
            altController.Screen.setCursor(2, 1);
            altController.Screen.print("Connected to: ");
            altController.Screen.setCursor(3, 1);

            if (Competition.isFieldControl()) {
                altController.Screen.print("Field");
            }
            if (Competition.isCompetitionSwitch()) {
                altController.Screen.print("Competition Switch");
            }
            if (!Competition.isFieldControl() && !Competition.isCompetitionSwitch()) {
                altController.Screen.print("Nothing");
            };
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

    cout << "ERROR: " << message << endl;
}

// Displays debug message on brain screen
void brainDebug(const char* message) {
  
    BrainLogs.newLog(message, vex::color::purple);

    cout << "DEBUG: " << message << endl;
}
void brainDebug(const char* message, bool notification) {
  
    BrainLogs.newLog(message, vex::color::purple);
    mainRenderer.newNotification(message, 4, purple);

    cout << "DEBUG: " << message << endl;
}

// Displays debug message on brain screen
void brainFancyDebug(const char* message, vex::color messageColor) {
  
    BrainLogs.newLog(message, messageColor);

    cout << "DEBUG: " << message << endl;
}

// Displays debug message on brain screen
// Has the ability to also show number info with the message
void brainFancyDebug(const char* message, vex::color messageColor, int data) {
  
    BrainLogs.newLog(message, messageColor, data);

    cout << "DEBUG: " << message << endl;
}

void brainFancyDebug(const char* message, vex::color messageColor, bool showNotification) {
  
    BrainLogs.newLog(message, messageColor);
    mainRenderer.newNotification(message, 4, messageColor);

    cout << "DEBUG: " << message << endl;
}