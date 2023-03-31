
#include "displayer.h"
#include "robotConfig.h"

#include "string"

using std::cout;
using std::endl;

using namespace vex;

// Todo: Make a better screen displaying system
// Make the brain show usefull info besides just the debug and error messages

/** 
 * What is happening in this file?
 * 
 * TLDR: This file is what controlls the display of the brain and the controllers
 * 
 * LogMessage Class
 *      This is the base log message object that is stored in the Logger's array. It stores its own
 *      message and display color 
 * 
 * Logger Class
 *      This class handles the log messages for the screen, it has auto scrolling and log saving support
 * 
 * ColorRange Class
 *      This is the color range object used for rendering. Instead of doing complicated math for gradients,
 *      the user (me) can define set ranges a certain color will display. By creating an array of these objects,
 *      the renderer will automatically choose the correct color for the item being displayed based on the input
 *      value and the ColorRange array passed into it
 * 
 * Drawer Class
 *      This class is what contains all of the fancy rendering fucntions. At the moment of writing this, it can 
 *      only render horizontal and vertical progress bars with ColorRange Arrays. 
 * 
 * The Error and Debug Functions at the end of the file
 *      These funcitons are what are shown to the rest of the program. Anywere in the code, those functions will
 *      be called and the LogMessage Class will handle everything else for displaying the info in the logs.
*/



// A base log message class
// It contains the actual text of the log and the display color
class LogMessage {
    private:

    public:
        std::string text;
        vex::color displayColor;
        int displayNumber;
    
        LogMessage(std::string message = "", vex::color messageColor = vex::color::white, int displayData = 0 ) {
            text = message;
            displayColor = messageColor;
            displayNumber = displayData;
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
            if (!Brain.SDcard.isInserted()) {
                return false; 
            }

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

        void newLog(const char* message, vex::color messageColor, int data = 0) {
            if (currentLine == maxRows) {
                brainLogShift();
                currentLine = maxRows - 1;
            }

            brainLog[currentLine] = LogMessage(message, messageColor, data);
            currentLine ++;

            writeLog(message);
        };

        void render() {
            
             // Display Log Messages
            for (int i = 0; i < maxRows; i++) {
                Brain.Screen.setCursor(i + yPos, xPos);
                Brain.Screen.setPenColor(brainLog[i].displayColor);
                Brain.Screen.print(brainLog[i].text.c_str(), brainLog[i].displayNumber);     
                Brain.Screen.setPenColor(vex::color::white);
            }
        };

};

// Stores a color and the range it is acceptable in
class colorRange {
    private:
        int start;
        int end;
        
    public:

        vex::color displayColor;

        colorRange(int rangeStart = 0, int rangeEnd = 0, vex::color showColor = vex::color::black) {
            start = rangeStart;
            end = rangeEnd;
            displayColor = showColor;
        };

        // Returns true if the value is in the range
        bool inRange(int value) {
            return (value >= start && value <= end);
        }
};

// The main element for drawing graphs and progress bars
class Drawer {
    private:

        // Used for custom color ranges
        // Returns the corresponding color based on the input value and the ranges array
        // Make sure to also pass in the size of the array, C++ is annoying and arrays decay to pointers once inputed into a function
        vex::color determinColorFromRange(int value, colorRange ranges[], int rangeSize) {
            for (int i = 0; i < rangeSize; i++) {
                if (ranges[i].inRange(value)) {
                    return ranges[i].displayColor;
                }
            }
            return vex::color::black;
        }

    public:

        Drawer(int test) {};

        // Draws a horizontal progress bar on the screen
        // Defaults to the battery percent color array unless declared by the user
        // If you want the name to display the value also use "%d" in the name
        void drawHorizontalProgressBar(int x, int y, int width, int height, const char* name, double value, bool middle = false, colorRange ranges[] = nullptr, int rangeSize = 0) {

            Brain.Screen.printAt(x, y, name, int(value));
            Brain.Screen.drawRectangle(x, y + 5, width, height);

            if (ranges && rangeSize != 0) {
                Brain.Screen.setFillColor(determinColorFromRange(value, ranges, rangeSize));
            } else {
                // Default Battery Color Range
                colorRange defaultRanges[3];
                defaultRanges[0] = colorRange(0, 20, red);
                defaultRanges[1] = colorRange(21, 35, yellow);
                defaultRanges[2] = colorRange(36, 100, green);

                Brain.Screen.setFillColor(determinColorFromRange(fabs(value), defaultRanges, 3));
            }
            

            if (middle) {
                Brain.Screen.drawRectangle(x + (width / 2), y + 5, (value / 100 ) * (width / 2), height); // Middle Draw
            } else {
                Brain.Screen.drawRectangle(x, y + 5, (value / 100) *width, height); // Normal Draw
            }
            Brain.Screen.setFillColor(vex::color::black);
        };


        // Draws a vertical progress bar on the screen
        // Defaults to the temperature color array unless declared by the user
        // If you want the name to display the value also use "%d" in the name
        void drawVerticalProgressbar(int x, int y, int width, int height, const char* name, double value, bool middle = false, colorRange ranges[] = nullptr, int rangeSize = 0) {

            Brain.Screen.printAt(x, y, name, int(value));
            Brain.Screen.drawRectangle(x, y + 5, width, height);
            
            if (ranges && rangeSize != 0) {
                Brain.Screen.setFillColor(determinColorFromRange(value, ranges, rangeSize));
            } else {
                // Default Heat Color Range
                colorRange defaultRanges[3];
                defaultRanges[0] = colorRange(0, 60, green);
                defaultRanges[1] = colorRange(61, 80, yellow);
                defaultRanges[2] = colorRange(81, 100, red);

                Brain.Screen.setFillColor(determinColorFromRange(fabs(value), defaultRanges, 3));
            }


            if (middle) {
                Brain.Screen.drawRectangle(x, y + 5 + (height / 2), width, - (value / 100) * height / 2);
            } else {
                Brain.Screen.drawRectangle(x, y + 5 + height, width, - (value / 100) * height);
            }
    
            Brain.Screen.setFillColor(vex::color::black);
        };
};


Drawer mainDrawer(1);
Logger BrainLogs(1, 1);



colorRange* gradientBuilder(double startHue, double endHue, int rangeStart, int rangeEnd, colorRange* finalGradient) {
    
    int steps = rangeEnd - rangeStart;

    double hueStep = (endHue - startHue) / steps;
    double hueTemp = startHue;

    
    vex::color tempColor;

    // Set the starting point
    tempColor.hsv(startHue, 1, 1);
    finalGradient[0] = colorRange(-10000, rangeStart, tempColor);   

    // Loop through everything in the middle 
    for (int i = 1; i < steps; i++) {

        hueTemp = hueTemp + hueStep;
        tempColor.hsv(hueTemp, 1, 1);
        finalGradient[i] = colorRange(rangeStart + i, rangeStart + i, tempColor);

        vex::wait(0.01, seconds);
    }

    // Set the ending point
    tempColor.hsv(endHue, 1, 1);
    finalGradient[199] = colorRange(rangeEnd, 10000, tempColor);

    return finalGradient;
}




// Main Loop For Rendering the Brain Display
int brainDisplayer() {

    Brain.Screen.clearScreen();
    Brain.Screen.newLine();
    Brain.Screen.print("Building Gradients");
    
    colorRange batteryGradient[200];    gradientBuilder(1, 100, 15, 70, batteryGradient);
    colorRange testGradient[200];       gradientBuilder(300, 180, -100, 100, testGradient);
    colorRange othertestGradient[200];  gradientBuilder(1, 240, -100, 100, othertestGradient);

    while(true) {

        Brain.Screen.clearScreen();

        // Render the logs to the screen
        BrainLogs.render();
        
        // Battery Level Meter
        mainDrawer.drawHorizontalProgressBar(230, 15, 150, 30, "Battery: %d%%", Brain.Battery.capacity(), false, batteryGradient, 100);

        // Test Vertical Meter
        mainDrawer.drawVerticalProgressbar(230, 75, 30, 100, "FL", sin((Brain.timer(vex::timeUnits::msec) / 1000) + 1) * 100, true);
        mainDrawer.drawVerticalProgressbar(270, 75, 30, 100, "FR", sin((Brain.timer(vex::timeUnits::msec) / 1000) + 2) * 100, true);
        mainDrawer.drawVerticalProgressbar(310, 75, 30, 100, "BL", sin((Brain.timer(vex::timeUnits::msec) / 1000) + 3) * 100, true);
        mainDrawer.drawVerticalProgressbar(350, 75, 30, 100, "BR", sin((Brain.timer(vex::timeUnits::msec) / 1000) + 4) * 100, true);

        mainDrawer.drawVerticalProgressbar(400, 15, 30, 200, "%d%%", fabs(sin((Brain.timer(vex::timeUnits::msec) / 1000) + 5)) * 100, false, batteryGradient, 100);
        mainDrawer.drawVerticalProgressbar(440, 15, 30, 200, "%d%%", fabs(sin((Brain.timer(vex::timeUnits::msec) / 1000) + 6)) * 100, false, testGradient, 100);

        mainDrawer.drawHorizontalProgressBar(230, 200, 150, 30, "Test: %d%%", sin(Brain.timer(vex::timeUnits::msec) / 1000) * 100, true, othertestGradient, 200);

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

        mainController.Screen.newLine();
        mainController.Screen.print(mainController.Axis2.position());


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