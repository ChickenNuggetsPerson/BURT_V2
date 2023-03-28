
#include "displayer.h"
#include "robotConfig.h"

#include "string"

using std::cout;
using std::endl;

using namespace vex;

// Todo: Make a better screen displaying system
// Make the brain show usefull info besides just the debug and error messages


// A base log message class
// It contains the actual text of the log and the display color
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

// The main class for controlling logging for the robot
// Handles displaying the logs on the screen and writing them to file
class Logger {
    private:

        int xPos;
        int yPos;

        int maxRows;
        int currentLine;

        LogMessage brainLog[11];

        bool savingLogs;

        void brainLogShift() {
            for (int i=0; i < maxRows - 1; i++) {
                brainLog[i] = brainLog[i + 1];
            }
        };

        bool initFileWriter() {
            if (!Brain.SDcard.isInserted()) { return false; }

            // Todo: Check for last log -> Arhive it ( somehow )
            //       Somehow create file write stream 

            return true;
        };

        void writeLog(const char* message) {
            if (!savingLogs) { return; }
            // Todo: Figure out how to append files correctly
        };

    public:
        Logger(int renderX, int renderY, int maxLogs = 11) {
            xPos = renderX;
            yPos = renderY;
            maxRows = maxLogs;
            savingLogs = initFileWriter();
        };

        void newLog(const char* message, vex::color messageColor) {
            if (currentLine == maxRows) {
                brainLogShift();
                currentLine = maxRows - 1;
            }

            LogMessage messageOBJ(message, messageColor);

            brainLog[currentLine] = messageOBJ;
            currentLine ++;

            writeLog(message);
        };

        void render() {
            
             // Display Log Messages
            for (int i = 0; i < maxRows; i++) {
                Brain.Screen.setCursor(i + yPos, xPos);
                Brain.Screen.setPenColor(brainLog[i].displayColor);
                Brain.Screen.print(brainLog[i].text.c_str());     
                Brain.Screen.setPenColor(vex::color::white);
            }
        };

};


Logger BrainLogs(1, 1);

// Main Loop For Rendering the Brain Display
int brainDisplayer() {
    while(true) {

        Brain.Screen.clearScreen();

        // Render the logs to the screen
        BrainLogs.render();

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

// Main Loop For Rendering the Controllers
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

// Displays error on brain screen
void brainError(const char* message) {
  
    BrainLogs.newLog(message, vex::color::red);

    cout << "ERROR: " << message << endl;
}

// Displays debug message on brain screen
void brainDebug(const char* message) {
  
    BrainLogs.newLog(message, vex::color::purple);

    cout << "DEBUG: " << message << endl;
}

// Displays debug message on brain screen
void brainDebugColor(const char* message, vex::color messageColor) {
  
    BrainLogs.newLog(message, messageColor);

    cout << "DEBUG: " << message << endl;
}