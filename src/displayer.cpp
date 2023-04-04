
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
        int start;
        int end;    
    public:
        vex::color displayColor;

        colorRange(int rangeStart = NAN, int rangeEnd = NAN, vex::color showColor = vex::color::black) {
            start = rangeStart;
            end = rangeEnd;
            displayColor = showColor;
        };

        // Returns true if the value is in the range
        bool inRange(int value) {
            return (value >= start && value <= end);
        }

        bool isDefault() {
            return start == NAN && end == NAN;
        }
};

class rgbColor {
    private:
    public:
        int r = 0;
        int g = 0;
        int b = 0;

        rgbColor(int red, int green, int blue) {
            r = red;
            g = green;
            b = blue;
        }

        rgbColor(const char* colorName) {
            if (strcmp(colorName, "red") == 0)    {
                r = 255; g = 0; b = 0;
            }
            if (strcmp(colorName, "orange") == 0) {
                r = 255; g = 166; b = 0;
            }
            if (strcmp(colorName, "yellow") == 0) {
                r = 255; g = 255; b = 0;
            }
            if (strcmp(colorName, "green") == 0)  {
                r = 0; g = 255; b = 0;
            }
            if (strcmp(colorName, "blue") == 0)   {
                r = 0; g = 0; b = 255;
            }
            if (strcmp(colorName, "purple") == 0) {
                r = 130; g = 0; b = 255;
            }
            if (strcmp(colorName, "white") == 0)  {
                r = 255; g = 255; b = 255;
            }
            if (strcmp(colorName, "black") == 0)  {
                r = 0; g = 0; b = 0;
            }
    }
};

// Handles generating smooth color gradients
class Gradient {
    private:
        int rangeStart;
        int rangeEnd;
        bool built = false;
    public:

        colorRange finalGradient[205];
        int rangeSize = 200;

        Gradient(double startHue, double endHue, int startOfRange, int endOfRange) {
            rangeStart = startOfRange;
            rangeEnd = endOfRange;     
            buildHSV(startHue, endHue);
        };
        void buildHSV(double startHue, double endHue) {

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

                //vex::wait(0.05, seconds);
            }

            // Set the ending point
            tempColor.hsv(endHue, 1, 1);
            finalGradient[i] = colorRange(rangeEnd, 10000, tempColor);

            built = true;

            rangeSize = i + 1;

            //brainDebug("Gradint Built");
        }

        Gradient(rgbColor startColor, rgbColor endColor, int startOfRange, int endOfRange) {
            rangeStart = startOfRange;
            rangeEnd = endOfRange;
            buildRGB(startColor, endColor);
        };
        void buildRGB(rgbColor startColor, rgbColor endColor) {

            if (built) { return; }
    
            int steps = rangeEnd - rangeStart;
    
            double redTemp = startColor.r;
            double greenTemp = startColor.g;
            double blueTemp = startColor.b;

            vex::color tempColor;

            // Set the starting point
            tempColor.rgb(redTemp, greenTemp, blueTemp);
            finalGradient[0] = colorRange(-10000, rangeStart, tempColor);   

            int i = 1;
            // Loop through everything in the middle 
            for (i = 1; i < steps; i++) {

                double percent = double(i) / double(steps);

                redTemp   = (endColor.r - startColor.r) * percent + startColor.r;
                greenTemp = (endColor.g - startColor.g) * percent + startColor.g;
                blueTemp  = (endColor.b - startColor.b) * percent + startColor.b;

                tempColor.rgb(redTemp, greenTemp, blueTemp);
                finalGradient[i] = colorRange(rangeStart + i, rangeStart + i, tempColor);

                //vex::wait(0.05, seconds);
            }

            // Set the ending point
            tempColor.rgb(endColor.r, endColor.g, endColor.b);
            finalGradient[i] = colorRange(rangeEnd, 10000, tempColor);

            rangeSize = i + 1;

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

        colorRange ranges[205];
        int rangeSize = 0;

        void shiftDataPoints() {
            for (int i=0; i < maxPoints - 1; i++) {
                dataPoints[i] = dataPoints[i + 1];
            }
        };
        
        vex::color determinColorFromRange(int value, colorRange ranges[]) {
            for (int i = 0; !ranges[i].isDefault(); i++) {
                if (ranges[i].inRange(value)) {
                    return ranges[i].displayColor;
                }  
            }
            return vex::color::black;
        }


    public:

        // Graph Types:
        //      1. Line Graph 

        void build(int graphType, const char* graphName, int xPos, int yPos, int graphWidth, int graphHeight, bool drawMiddle, colorRange colorRanges[] = nullptr, int maxDataPoints = 10) {
            type = graphType;
            x = xPos;
            y = yPos;
            width = graphWidth;
            height = graphHeight;
            name = graphName;
            maxPoints = maxDataPoints;

            for (int i = 0; i < 206; i++) {
                if (!ranges[i].isDefault()) {
                   ranges[i] = colorRanges[i]; 
                } else { break; }
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
                    Brain.Screen.setPenColor(determinColorFromRange(dataPoints[i], ranges));
                }

                Brain.Screen.drawLine(x + (xScale * i), y + 5 + height*middle - ((dataPoints[i] / 100.00) * height)*middle, x + (xScale * i), y + 5 + height*middle);
                Brain.Screen.setPenColor(vex::color::white);
            };

        };
};

struct progressBar {

    const char* id;

    bool vertical;
    int x;
    int y;
    int width;
    int height;
    const char* name;
    double value;
    bool middle = false;
    colorRange ranges[205];
};

class Page {
    private:

        progressBar barStorage[10];
        int barsStored = 0;

        Graph graphStorage[10];
        const char* graphIdStorage[10];
        int graphsStored = 0;

        Logger* loggerStorage;
        bool hasLogger = false;


        vex::color determinColorFromRange(int value, colorRange ranges[]) {
            for (int i = 0; !ranges[i].isDefault(); i++) {
                if (ranges[i].inRange(value)) {
                    return ranges[i].displayColor;
                }  
            }
            return vex::color::black;
        }
        
        void drawHorzProgressbar(progressBar bar) {
            Brain.Screen.printAt(bar.x, bar.y, bar.name, int(bar.value));
            Brain.Screen.drawRectangle(bar.x, bar.y + 5, bar.width, bar.height);

            if (!bar.ranges[0].isDefault()) {
                Brain.Screen.setFillColor(determinColorFromRange(bar.value, bar.ranges));
            } else {
                // Default Battery Color Range
                colorRange defaultRanges[4];
                defaultRanges[0] = colorRange(0, 20, red);
                defaultRanges[1] = colorRange(21, 35, yellow);
                defaultRanges[2] = colorRange(36, 100, green);

                Brain.Screen.setFillColor(determinColorFromRange(fabs(bar.value), defaultRanges));
            }
            

            if (bar.middle) {
                Brain.Screen.drawRectangle(bar.x + (bar.width / 2), bar.y + 5, (bar.value / 100 ) * (bar.width / 2), bar.height); // Middle Draw
            } else {
                Brain.Screen.drawRectangle(bar.x, bar.y + 5, (bar.value / 100) * bar.width, bar.height); // Normal Draw
            }
            Brain.Screen.setFillColor(vex::color::black);
        };
        void drawVertProgressBar(progressBar bar) {
            Brain.Screen.printAt(bar.x, bar.y, bar.name, int(bar.value));
            Brain.Screen.drawRectangle(bar.x, bar.y + 5, bar.width, bar.height);
            
            if (!bar.ranges[0].isDefault()) {
                Brain.Screen.setFillColor(determinColorFromRange(bar.value, bar.ranges));
            } else {
                // Default Heat Color Range
                colorRange defaultRanges[4];
                defaultRanges[0] = colorRange(0, 60, green);
                defaultRanges[1] = colorRange(61, 80, yellow);
                defaultRanges[2] = colorRange(81, 100, red);

                Brain.Screen.setFillColor(determinColorFromRange(fabs(bar.value), defaultRanges));
            }


            if (bar.middle) {
                Brain.Screen.drawRectangle(bar.x, bar.y + 5 + (bar.height / 2), bar.width, - (bar.value / 100) * bar.height / 2);
            } else {
                Brain.Screen.drawRectangle(bar.x, bar.y + 5 + bar.height, bar.width, - (bar.value / 100) * bar.height);
            }
    
            Brain.Screen.setFillColor(vex::color::black);
        };



    public:
        Page(int test) {};

        void render() {

            if (hasLogger) {
                loggerStorage->render();
            }

            for (int i = 0; i < barsStored; i++) {
                if (barStorage[i].vertical) {
                    drawVertProgressBar(barStorage[i]);
                } else {
                    drawHorzProgressbar(barStorage[i]);
                }
            }

            for (int i = 0; i < graphsStored; i++) {
                graphStorage[i].draw();
            }
        };


        void addLogger(Logger* loggerMemLocation) {
            loggerStorage = loggerMemLocation;
            hasLogger = true;
        };
        void addHorzProgressBar(const char* barId, int x, int y, int width, int height, const char* name, bool middle = false, colorRange ranges[] = nullptr) {
            progressBar tempBar;
            tempBar.x = x;
            tempBar.y = y;
            tempBar.width = width;
            tempBar.height = height;
            tempBar.name = name;
            tempBar.middle = middle;
            tempBar.vertical = false;
            tempBar.id = barId;

            for (int i = 0; i < 206; i++) {
                if (!ranges[i].isDefault()) {
                   tempBar.ranges[i] = ranges[i]; 
                } else { break; }
            }

            barStorage[barsStored] = tempBar;
            barsStored++;

        };
        void addVertProgressBar(const char* barId, int x, int y, int width, int height, const char* name, bool middle = false, colorRange ranges[] = nullptr) {
            progressBar tempBar;
            tempBar.x = x;
            tempBar.y = y;
            tempBar.width = width;
            tempBar.height = height;
            tempBar.name = name;
            tempBar.middle = middle;
            tempBar.vertical = true;
            tempBar.id = barId;

            for (int i = 0; i < 206; i++) {
                if (!ranges[i].isDefault()) {
                   tempBar.ranges[i] = ranges[i]; 
                } else { break; }
            }

            barStorage[barsStored] = tempBar;
            barsStored++;
        };
        void addLineGraph(const char* graphId, int graphType, const char* graphName, int xPos, int yPos, int graphWidth, int graphHeight, bool drawMiddle, colorRange colorRanges[] = nullptr, int maxDataPoints = 10) {
            graphStorage[graphsStored] = Graph();
            graphStorage[graphsStored].build(graphType, graphName, xPos, yPos, graphWidth, graphHeight, drawMiddle, colorRanges, maxDataPoints);
            graphIdStorage[graphsStored] = graphId;
            graphsStored++;
        };

        void setProgressBarValue(const char* barId, int value) {
            for (int i = 0; i < barsStored; i++) {
                if (strcmp(barId, barStorage[i].id) == 0) {
                    barStorage[i].value = value;
                }
            }
        };
        void setLineGraphValue(const char* graphId, int value) {
            for (int i = 0; i < graphsStored; i++) {
                if (strcmp(graphId, graphIdStorage[i]) == 0) {
                    graphStorage[i].addPoint(value);
                }
            }
        };



};



// Define all elements 

Logger BrainLogs(1, 1);
Page homePage(1);
double graphUpdateRate = 0.05; // In Seconds



int debugDataUpdater() {

    // Init Gradients
    Gradient batteryGradient = Gradient(1, 100, 15, 70);
    Gradient graphGradient = Gradient(10, 300, 10, 100);
    Gradient rainbowGradient = Gradient(rgbColor(250, 100, 100), rgbColor(237, 212, 252), 0, 100);

    homePage.addLogger(&BrainLogs);

    homePage.addHorzProgressBar("battery", 325, 15, 150, 30, "Battery: %d%%", false, batteryGradient.finalGradient);
    homePage.addVertProgressBar("testVert", 280, 15, 30, 100, "%d", false, batteryGradient.finalGradient);
    homePage.addVertProgressBar("otherTestVert", 230, 15, 30, 100, "%d", false, rainbowGradient.finalGradient);

    homePage.addLineGraph("test", 1, "Graphypoo %d%%", 325, 150, 150, 75, false, rainbowGradient.finalGradient, 50);
    homePage.addLineGraph("othertest", 1, "Other Graph: %d%%", 150, 150, 150, 75, false, graphGradient.finalGradient, 50);



    while(true) {

        // Update Data on screen

        double time = Brain.timer(vex::timeUnits::msec) / 1000;
        double  value = int(sin(time)*50) + 50;

        homePage.setLineGraphValue("test", value);
        homePage.setLineGraphValue("othertest", mainController.Axis3.position());

        homePage.setProgressBarValue("battery", Brain.Battery.capacity());
        homePage.setProgressBarValue("testVert", value);
        homePage.setProgressBarValue("otherTestVert", 100 - value);

        wait(graphUpdateRate, seconds);
    }

    return 1;
};

// Main Loop For Rendering the Brain Display
int brainDisplayer() {


    double waitTime = 1; // Seconds    

    double endTime = Brain.timer(vex::timeUnits::msec) + (waitTime * 1000);
    while (endTime > Brain.timer(vex::timeUnits::msec)) {
        Brain.Screen.clearScreen();
        Brain.Screen.printAt(20, 40, "Calibrating... Do not touch the robot");
        
        char barArray[40];
        int arrayLength = sizeof(barArray) / sizeof(char);
        barArray[0] = '[';
        barArray[arrayLength] = ']';

        double currentPercent = 1 - ((endTime - Brain.timer(msec)) / (waitTime * 1000));

        for (int i = 1; i < arrayLength; i++) {
            if ((double(i) / double(arrayLength)) < currentPercent) {
                barArray[i] = '=';
            } else {
                barArray[i] = ' ';
            }
        }
        Brain.Screen.printAt(20, 80, barArray);
        Brain.Screen.render();
    }
    
    
    double deltaTime = 0.00;
    while(true) {
        double startTime = Brain.timer(msec);
        Brain.Screen.clearScreen();
        // Show the screen FPS
        Brain.Screen.printAt(1, 235, "FPS: %d", int(deltaTime));
        homePage.render();
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