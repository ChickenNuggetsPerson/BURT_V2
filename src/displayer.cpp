
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
 * Gradient Class
 *      This class helps handle automatically creating a smooth color gradient for the display bars. When created,
 *      it is given two points [-100, 100] and two colors. Since going through and building the gradient can take a bit, 
 *      the .build() method needs to be ran first. Before building the gradient, it will just display as a black bar. 
 * 
 * Drawer Class
 *      This class is what contains all of the fancy rendering fucntions. At the moment of writing this, it can 
 *      only render horizontal and vertical progress bars with ColorRange Arrays. 
 * 
 * The Error and Debug Functions at the end of the file
 *      These funcitons are what are shown to the rest of the program. Anywere in the code, those functions will
 *      be called and the LogMessage Class will handle everything else for displaying the info in the logs.
 * 
 * 
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
    
    public:

        int start;
        int end;

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
class ProgressBarDrawer {
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

        ProgressBarDrawer(int test) {};

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

// Handles generating smooth color gradients
class Gradient {
    private:
        double startHue;
        double endHue;
        int rangeStart;
        int rangeEnd;
        bool built = false;
    public:

        colorRange finalGradient[200];

        Gradient(double startColor, double endColor, int startOfRange, int endOfRange) {
            startHue = startColor;
            endHue = endColor;
            rangeStart = startOfRange;
            rangeEnd = endOfRange;     
        };

        void build() {

            if (built) { return; }
    
            int steps = rangeEnd - rangeStart;

            double hueStep = (endHue - startHue) / steps;
            double hueTemp = startHue;
    
            vex::color tempColor;

            // Set the starting point
            tempColor.hsv(startHue, 1, 1);
            finalGradient[0] = colorRange(-10000, rangeStart, tempColor);   

            int i = 1;
            // Loop through everything in the middle 
            for (i = 1; i < steps; i++) {

                hueTemp = hueTemp + hueStep;
                tempColor.hsv(hueTemp, 1, 1);
                finalGradient[i] = colorRange(rangeStart + i, rangeStart + i, tempColor);

                //vex::wait(0.01, seconds);
            }

            // Set the ending point
            tempColor.hsv(endHue, 1, 1);
            finalGradient[i] = colorRange(rangeEnd, 10000, tempColor);

            built = true;
        }
};

// Handles displaying data over time on the screen
class Graph {
    private:
        int type;
        int x;
        int y;
        int width;
        int height;
        const char* name;
        double middle = 1;

        int dataPoints[100] = {0};
        int maxPoints;
        int currentDataPoint = 0;
        int recentPoint = 0;

        colorRange ranges[202];
        int rangeSize = 0;

        void shiftDataPoints() {
            for (int i=0; i < maxPoints - 1; i++) {
                dataPoints[i] = dataPoints[i + 1];
            }
        };
        
        vex::color determinColorFromRange(int value, colorRange ranges[], int rangeSize) {
            for (int i = 0; i < rangeSize; i++) {
                if (ranges[i].inRange(value)) {
                    return ranges[i].displayColor;
                }
            }
            return vex::color::white;
        }


    public:

        // Graph Types:
        //      1. Line Graph 

        void build(int graphType, const char* graphName, int xPos, int yPos, int graphWidth, int graphHeight, bool drawMiddle, colorRange colorRanges[] = nullptr, int sizeOfRange = 0, int maxDataPoints = 10) {
            type = graphType;
            x = xPos;
            y = yPos;
            width = graphWidth;
            height = graphHeight;
            name = graphName;
            maxPoints = maxDataPoints;

            rangeSize = sizeOfRange;
            for (int i = 0; i < sizeOfRange; i++) {
                ranges[i] = colorRanges[i];
            }

            if (maxPoints > 50) {maxPoints = 50;}
            if (drawMiddle) {
                middle = 0.5;
            }
        };

        void addPoint(int point) {
            if (currentDataPoint == maxPoints) {
                shiftDataPoints();
                currentDataPoint = maxPoints - 1;
            }
            dataPoints[currentDataPoint] = point;
            currentDataPoint ++;

            recentPoint = point;
        };

        void draw() {
            
            Brain.Screen.printAt(x, y, name, recentPoint);
            Brain.Screen.drawRectangle(x, y + 5, width, height);

            double xScale = width / maxPoints;
            double yScale = height / 10;

            //brainFancyDebug("x: %d", x);
            //brainFancyDebug("y: %d", y);
            //brainFancyDebug("w: %d", width);
            //brainFancyDebug("h: %d", height);

            // Draw Vertical Axis
            for (int i = 0; i < 10; i++) {
                Brain.Screen.drawLine(x, y + 5 + (i * yScale), x + 5, y + 5 + (i * yScale));
            };

            // Adjusted to handle negative values

            // Draw X Axis
            Brain.Screen.drawLine(x, y + 5 + height*middle, x + width, y + 5 + height*middle);
            if (middle == 1) {
                for (int i = 0; i < maxPoints; i++) {
                    Brain.Screen.drawLine(x + (xScale * i), y + 5 + height, x + (xScale * i), y + 1 + height);
                };
            } else {
                for (int i = 0; i < maxPoints; i++) {
                    Brain.Screen.drawLine(x + (xScale * i), y + 4 + height*middle, x + (xScale * i), y + 6 + height*middle);
                };
            }

            // Draw Lines
            for (int i = 0; i < maxPoints; i++) {
                
                if (i + 1 != currentDataPoint) {
                    Brain.Screen.drawLine(x + (xScale * i), y + 5 + height*middle - ((dataPoints[i] / 100.00) * height)*middle, x + (xScale * (i + 1)), y + 5 + height*middle - ((dataPoints[i + 1] / 100.00) * height*middle));
                } else {
                    Brain.Screen.drawCircle(x + (xScale * i), y + 5 + height*middle - ((dataPoints[i] / 100.00) * height)*middle, 3);
                }

                if (xScale > 20.00) {
                    Brain.Screen.drawCircle(x + (xScale * i), y + 5 + height*middle - ((dataPoints[i] / 100.00) * height)*middle, 3);
                }

                if (rangeSize != 0 && i != 0) {
                    Brain.Screen.setPenColor(determinColorFromRange(dataPoints[i], ranges, rangeSize));
                }

                Brain.Screen.drawLine(x + (xScale * i), y + 5 + height*middle - ((dataPoints[i] / 100.00) * height)*middle, x + (xScale * i), y + 5 + height*middle);
                Brain.Screen.setPenColor(vex::color::white);
            };

            

        };
};



// Define all elements 

ProgressBarDrawer progressDrawer(1);
Logger BrainLogs(1, 1);

double graphUpdateRate = 0.02;

// Gradients
Gradient batteryGradient = Gradient(1, 100, 15, 70);
Gradient testGradient = Gradient(0, 360, -100, 100);

Gradient graphGradient = Gradient(10, 240, -90, 90);
Gradient rainbowGradient = Gradient(0, 360, -100, 100);


// Graphs
Graph testGraph;


int debugDataUpdater() {

    batteryGradient.build();
    testGradient.build();
    graphGradient.build();
    rainbowGradient.build();

    testGraph.build(1, "Graphypoo %d%%", 325, 80, 150, 150, true, graphGradient.finalGradient, 202, 50);

    while(true) {

        double time = Brain.timer(vex::timeUnits::msec) / 1000;

        double value = int(sin(tan(time)) * sin(time) * 100);

        testGraph.addPoint(value);
        //testGraph.addPoint(mainController.Axis3.position());

        wait(graphUpdateRate, seconds);
    }

    return 1;
};

// Main Loop For Rendering the Brain Display
int brainDisplayer() {

    double deltaTime = 0.00;

    while(true) {

        double startTime = Brain.timer(msec);

        Brain.Screen.clearScreen();

        Brain.Screen.printAt(1, 235, "FPS: %d", int(deltaTime));

        // Render the logs to the screen
        BrainLogs.render();
        
        // Battery Level Meter
        progressDrawer.drawHorizontalProgressBar(325, 15, 150, 30, "Battery: %d%%", Brain.Battery.capacity(), false, batteryGradient.finalGradient, 100);

        // Two test meters
        progressDrawer.drawVerticalProgressbar(230, 15, 30, 60, "%d%%", mainController.Axis3.position(), true, batteryGradient.finalGradient, 200);
        progressDrawer.drawVerticalProgressbar(270, 15, 30, 60, "%d%%", (sin((Brain.timer(vex::timeUnits::msec) / 1000) + 6) * 100), true, testGradient.finalGradient, 200);

        progressDrawer.drawHorizontalProgressBar(230, 100, 85, 30, "X: %d", mainController.Axis4.position(), true, rainbowGradient.finalGradient, 201);

        // Draw the graph
        testGraph.draw();


        // Calculate the fps
        deltaTime = 1000 / (round(Brain.timer(msec) - startTime));
        
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