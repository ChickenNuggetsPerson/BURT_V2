
#include "displayer.h"
#include "robotConfig.h"

#include "string"

using std::cout;
using std::endl;

using namespace vex;

// Todo: Make a better screen displaying system
// Make the brain show usefull info besides just the debug and error messages


class LogMessage {
    private:

    public:
        std::string text;
        vex::color displayColor;
    
        LogMessage(std::string message = "", vex::color messageColor = vex::color::white) {
            text = message;
            displayColor = messageColor;

            
        };
};


int brainLogMaxRows = 11;
LogMessage brainLog[11];
int brainLogCurrentLine;

int brainDisplayer() {
    while(true) {

        Brain.Screen.clearScreen();

        for (int i = 0; i < brainLogMaxRows; i++) {
            Brain.Screen.setCursor(i + 1, 1);
            Brain.Screen.setPenColor(brainLog[i].displayColor);
            Brain.Screen.print(brainLog[i].text.c_str());     
            Brain.Screen.setPenColor(vex::color::white);
        }


        // Battery Level Meter
        Brain.Screen.printAt(230, 15, "Battery Level: %d%%", Brain.Battery.capacity());
        Brain.Screen.drawRectangle(230, 20, 250, 30);
        if (Brain.Battery.capacity() < 50) {
            if (Brain.Battery.capacity() <= 30) {
                Brain.Screen.setFillColor(vex::color::red);
            } else {
                Brain.Screen.setFillColor(vex::color::yellow);
            }
        } else {
            Brain.Screen.setFillColor(vex::color::green);
        }
        Brain.Screen.drawRectangle(230, 20, Brain.Battery.capacity()*2.5, 30);
        Brain.Screen.setFillColor(vex::color::black);

        // Render the screen
        Brain.Screen.render();

    }

    return 1;
}


int controllerDisplay() {
    while (true){
        
        // Main Controller Displayer
        mainController.Screen.clearScreen();
        mainController.Screen.setCursor(1, 1);
        mainController.Screen.print("Juice Left: ");
        mainController.Screen.print(Brain.Battery.capacity(percent));

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
        
        vex::wait(1, seconds);
    }
    return 1;
}

void brainLogShift() {
    for (int i=0; i < brainLogMaxRows - 1; i++) {
        brainLog[i] = brainLog[i + 1];
    }
}



// Displays error on brain screen
void brainError(const char* message) {
  
    if (brainLogCurrentLine == brainLogMaxRows) {
        brainLogShift();
        brainLogCurrentLine = brainLogMaxRows - 1;
    }

    LogMessage messageOBJ(message, vex::color::red);

    brainLog[brainLogCurrentLine] = messageOBJ;
    brainLogCurrentLine ++;

    cout << "ERROR: " << message << endl;
}

// Displays debug message on brain screen
void brainDebug(const char* message) {
  
  
    if (brainLogCurrentLine == brainLogMaxRows) {
        brainLogShift();
        brainLogCurrentLine = brainLogMaxRows - 1;
    }

    LogMessage messageOBJ(message, vex::color::purple);

    brainLog[brainLogCurrentLine] = messageOBJ;
    brainLogCurrentLine ++;

    cout << "DEBUG: " << message << endl;
}

// Displays debug message on brain screen
void brainDebugColor(const char* message, vex::color messageColor) {
  
  
    if (brainLogCurrentLine == brainLogMaxRows) {
        brainLogShift();
        brainLogCurrentLine = brainLogMaxRows - 1;
    }

    LogMessage messageOBJ(message, messageColor);

    brainLog[brainLogCurrentLine] = messageOBJ;
    brainLogCurrentLine ++;

    cout << "DEBUG: " << message << endl;
}