#include "vex.h"
#include "string"
#include "robotConfig.h"
#include "vector"

using namespace vex;

// A base log message class
// It contains the actual text of the log and the display color
struct LogMessage {
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

        bool initialized = false;

        int xPos;
        int yPos;

        int maxRows;
        int currentLine;

        const char* logFile;
        bool isSaving = false;
        bool compLogs = false;
        const char* compLogPath;

        std::vector <LogMessage> brainLog;

        void brainLogShift() {
            brainLog.erase(brainLog.begin());
        };

        void appendLogFile(const char* message) {
            if (!isSaving) { return; }

            std::ostringstream formattedMessage;
            formattedMessage << "[ " << Brain.timer(seconds) << " ] " << message;

            appendFile(logFile, formattedMessage.str().c_str());
            if (compLogs) {
                appendFile(compLogPath, formattedMessage.str().c_str()); // Todo: Fix this 
            }
        };

    public:
        Logger(int renderX, int renderY, const char* outFile = "logs.txt", int maxLogs = 11) {
            xPos = renderX;
            yPos = renderY;
            maxRows = maxLogs;

            logFile = outFile;
            isSaving = Brain.SDcard.isInserted();
            writeFile(logFile, "Start of Logs");

            //reloadLogger(logFile);
            
        };

        void reloadLogger(const char* outFile = "logs.txt") {
            logFile = outFile;
            isSaving = Brain.SDcard.isInserted();
            if (!isSaving) { 
                initialized = true;   
                return; 
            }

            compLogs = Competition.isFieldControl();

            std::string path;
            path += systemConfigFolder;
            path += systemArchivePath;
            if (readFile(path.c_str()) == 1) {
                std::cout << std::endl << std::endl << std::endl;
                if (fileExists(logFile)) {
                    // Save the last logs to the archive folder
                    int logNum = 0;
                    bool looking = true;
                    std::cout << "Starting Log Looking" << std::endl;
                    while (looking) {
                        logNum ++;
                        std::string tmpName;
                        tmpName += systemLogFolder;
                        tmpName += systemArchiveFolder;
                        
                        std::ostringstream str;
                        str << logNum;
                        tmpName += str.str();

                        tmpName += ".txt";
                        //std::cout << "Checking " << tmpName.c_str() << std::endl;
                        looking = fileExists(tmpName.c_str());
                    }
                    std::string cpyDest;
                    cpyDest += systemLogFolder;
                    cpyDest += systemArchiveFolder;

                    std::ostringstream str;
                    str << logNum;
                    cpyDest += str.str();

                    cpyDest += ".txt";

                    std::cout << "Archiving to:  " << cpyDest << std::endl;

                    copyFile(logFile, cpyDest.c_str());
                }
            } else {
                std::cout << std::endl << std::endl << "Not Archiving" << std::endl;
            }
            if (compLogs) {
                int logNum = 0;
                bool looking = true;
                std::cout << "Starting Comp Log Looking" << std::endl;
                while (looking) {
                    logNum ++;
                    std::string tmpName;
                    tmpName += systemLogFolder;
                    tmpName += systemCompLogsFolder;
                    std::ostringstream str;
                    str << logNum;
                    tmpName += str.str();
                    tmpName += ".txt";
                    //std::cout << "Checking " << tmpName.c_str() << std::endl;
                    looking = fileExists(tmpName.c_str());
                }
                std::string cpyDest;
                cpyDest += systemLogFolder;
                cpyDest += systemCompLogsFolder;
                std::ostringstream str;
                str << logNum;
                cpyDest += str.str();
                cpyDest += ".txt";

                compLogPath = cpyDest.c_str();

                std::cout << "Archiving To: " << compLogPath << std::endl;

                writeFile(compLogPath, "Start of Comp Logs");

            }

            writeFile(logFile, "Start of Logs");
            initialized = true;  
        }

        void newLog(const char* message, vex::color messageColor, int data = 0) {

            if (currentLine == maxRows) {
                brainLogShift();
                currentLine = maxRows - 1;
            }

            brainLog.push_back(LogMessage(message, messageColor, data));
            currentLine ++;

            appendLogFile(message);
        };

        void render() {
            
             // Display Log Messages
            for (int i = 0; i < brainLog.size(); i++) {
                Brain.Screen.setCursor(i + yPos, xPos);
                Brain.Screen.setPenColor(brainLog.at(i).displayColor);
                Brain.Screen.print(brainLog.at(i).text.c_str(), brainLog.at(i).displayNumber);     
                Brain.Screen.setPenColor(vex::color::white);
            }
        };


        const char* getLogPath(bool compPath = false) {
            if (!compPath) {
                return logFile;
            } else {
                return compLogPath;
            }
        }

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

        std::vector<colorRange> finalGradient;

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
            finalGradient.push_back(colorRange(-10000, rangeStart, tempColor));   

            int i = 1;
            // Loop through everything in the middle 
            for (i = 1; i < steps; i++) {

                hueTemp = hueTemp + hueStep;
                tempColor.hsv(hueTemp, 1, 1);
                finalGradient.push_back(colorRange(rangeStart + i, rangeStart + i, tempColor));

                //vex::wait(0.05, seconds);
            }

            // Set the ending point
            tempColor.hsv(endHue, 1, 1);
            finalGradient.push_back(colorRange(rangeEnd, 10000, tempColor));

            built = true;
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
            finalGradient.push_back(colorRange(-10000, rangeStart, tempColor));   

            int i = 1;
            // Loop through everything in the middle 
            for (i = 1; i < steps; i++) {

                double percent = double(i) / double(steps);

                redTemp   = (endColor.r - startColor.r) * percent + startColor.r;
                greenTemp = (endColor.g - startColor.g) * percent + startColor.g;
                blueTemp  = (endColor.b - startColor.b) * percent + startColor.b;

                tempColor.rgb(redTemp, greenTemp, blueTemp);
                finalGradient.push_back(colorRange(rangeStart + i, rangeStart + i, tempColor));

                //vex::wait(0.05, seconds);
            }

            // Set the ending point
            tempColor.rgb(endColor.r, endColor.g, endColor.b);
            finalGradient.push_back(colorRange(rangeEnd, 10000, tempColor));

            built = true;
        }
};

// Handles displaying data over time on the screen
class Graph {
    private:

        const char* graphId;
        
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

        std::vector<colorRange> ranges;

        void shiftDataPoints() {
            for (int i=0; i < maxPoints - 1; i++) {
                dataPoints[i] = dataPoints[i + 1];
            }
        };
        
        vex::color determinColorFromRange(int value, std::vector<colorRange> ranges) {
            for (auto range: ranges) {
                if (range.inRange(value)) {
                    return range.displayColor;
                }  
            }
            return vex::color::black;
        }


    public:

        // Graph Types:
        //      1. Line Graph 

        const char* getId() {
            return graphId;
        };

        void build(const char* graphId, const char* graphName, int xPos, int yPos, int graphWidth, int graphHeight, bool drawMiddle, std::vector<colorRange> colorRanges, int maxDataPoints = 10) {
            this->graphId = graphId;
            this->x = xPos;
            this->y = yPos;
            this->width = graphWidth;
            this->height = graphHeight;
            this->name = graphName;
            this->maxPoints = maxDataPoints;

            this->ranges = colorRanges;

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

                if (ranges.size() != 0 && i != 0) {
                    Brain.Screen.setPenColor(determinColorFromRange(dataPoints[i], ranges));
                }

                Brain.Screen.drawLine(x + (xScale * i), y + 5 + height*middle - ((dataPoints[i] / 100.00) * height)*middle, x + (xScale * i), y + 5 + height*middle);
                Brain.Screen.setPenColor(vex::color::white);
            };

        };
};

struct ProgressBar {
    const char* id;
    bool vertical;
    int x;
    int y;
    int width;
    int height;
    const char* name;
    double value;
    bool middle = false;
    std::vector<colorRange> ranges;
    
    ProgressBar(const char* barId, int x, int y, int width, int height, const char* name, bool vertical, bool middle, std::vector<colorRange> ranges) {
        this->id = barId;
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
        this->name = name;
        this->middle = middle;
        this->vertical = vertical;
        if (ranges.size() != 0) {
            this->ranges = ranges;
        }
    }
};

struct DisplayText {
    const char* text;
    int displayData = NAN;
    double displayDoubleData = NAN;
    bool displayInt = true;
    int x;
    int y;
    const char* id;
    vex::color displayColor;
    fontType displayFont;
    DisplayText(const char* text, int x, int y, vex::color displayColor = color::white, vex::fontType displayFont = fontType::mono20, const char* id = "") {
        this->text = text;
        this->x = x;
        this->y = y;
        this->displayData = 0;
        this->displayColor = displayColor;
        this->displayFont = displayFont;
        this->id = id;
        
    }
};

struct DisplayBox {
    int x;
    int y;
    int width;
    int height;
    const char* id;
    vex::color fillColor;
    vex::color penColor;
    DisplayBox(const char* id, int x, int y, int width, int height, vex::color fillColor, vex::color penColor = color::white) {
        this->id = id;
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
        this->fillColor = fillColor;
        this->penColor = penColor;
    }
};

class Page; // Forward decloration so the button class can build
class MenuSystem;
class AdjustableNum;

struct Button {
    int x;
    int y;
    int width;
    int height;
    const char* id;
    const char* text;
    int data;
    vex::color fillColor;
    int (*cb)(Page*);
    Button(const char* text, int x, int y, int width, int height, int (*cb)(Page*), const char* id = "", vex::color fillColor = black) {
        this->text = text;
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
        this->cb = cb;
        this->id = id;
        this->fillColor = fillColor;
    }
};

struct OverlayQuestion {
    std::string question;    
    const char* option1;
    vex::color option1Color = color::white;    
    const char* option2;
    vex::color option2Color = color::white;
};

struct Toggle {
    const char* id;

    const char* displayText;
    const char* trueText;
    bool showAlt = false;

    bool status;
    vex::color offColor;
    vex::color onColor;
    int x;
    int y;
    int width;
    int height;
    Toggle(const char* id, const char* displayText, bool startStatus, vex::color offColor, vex::color onColor, int x, int y, int width, int height, const char* trueText = "") {
        this->id = id;
        this->displayText = displayText;
        this->status = startStatus;
        this->offColor = offColor;
        this->onColor = onColor;
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
        this->trueText = trueText;
        if (strcmp(trueText, "") != 0) {
            this->showAlt = true;
            this->trueText = trueText;
        }
    }
};


class AdjustableNum {
    private:

        const char* title;
        Page* pagePtr;

        double val = 0.00;
        double stepSize = 0.00;
        double maxVal = 0.00;
        double minVal = 0.00;

        int x,y,width,height = 0;
        
        bool showRange = false;

        vex::fontType font = vex::fontType::mono20;
    
        bool inRectangle(int posX, int posY, int x, int y, int width, int height) {
            return (posX > x && posX < x + width) && (posY > y && posY < y + height);
        };

    public:
        AdjustableNum() {};
        AdjustableNum(const char* id, double initialVal, double stepAmount, double maxAmount, double minAmount, int displayX, int displayY, int displayWidth, int displayHeight, vex::fontType displayFont, Page* ptr, bool showingRange) {
            title = id;
            val = initialVal;
            stepSize = stepAmount;
            maxVal = maxAmount;
            minVal = minAmount;
            x = displayX;
            y = displayY;
            width = displayWidth;
            height = displayHeight;
            font = displayFont;
            pagePtr = ptr;
            showRange = showingRange;
        }

        void render() {
    
            std::stringstream displayValStream;
            displayValStream << val;
            const char* displayVal = displayValStream.str().c_str();

            Brain.Screen.drawRectangle(x, y, width, height);
            Brain.Screen.setFont(font);
            Brain.Screen.printAt(x + (width / 2) - (Brain.Screen.getStringWidth(displayVal) / 2), y + (height / 2) + (Brain.Screen.getStringHeight(displayVal) / 4), displayVal);

        }
        bool checkPressed(int posX, int posY) {
            return inRectangle(posX, posY, x, y, width, height);
        }

        const char* getId() {
            return title;
        }

        void increase() {
            val += stepSize;
            if (val > maxVal) { val = maxVal; }
        };
        void decrease() {
            val -= stepSize;
            if (val < minVal) { val = minVal; }
        }

        double getVal() {
            return val;
        }
        void setVal(double newVal) {
            double tmpVal = newVal;
            if (tmpVal > maxVal) { tmpVal = maxVal; }
            if (tmpVal < minVal) { tmpVal = minVal; }
            val = tmpVal;
        }
        std::string getDisplayVal() {
            if (showRange) {
                std::ostringstream stream1;
                stream1 << minVal;
                std::ostringstream stream2;
                stream2 << " - " << val ;
                std::ostringstream stream3;
                stream3 << " - " << maxVal;
                std::string finalString;

                finalString += stream1.str();
                finalString += stream2.str();
                finalString += stream3.str();

                return finalString;
            } else {
                std::stringstream displayValStream;
                displayValStream << val;
                return displayValStream.str();
            }
            
        }

};


class Plot {

    private:

        MenuSystem* systemPtr;
        Position posAtDist(Position currentPosition, int dist) {
            double desiredX = currentPosition.x + dist * cos(2.5*PI - currentPosition.rot);
            double desiredY = currentPosition.y + dist * sin(2.5*PI - currentPosition.rot);
            return Position(desiredX, desiredY);
        }

    public:
        const char* id;
        const char* label;
        int x;
        int y;
        int width;
        int height;
        int maxX;
        int maxY;
        int subdiv;
        bool labels;


        Position point1;
        bool drawPoint1 = false;
        Position point2;
        bool drawPoint2 = false;

        bool drawStoredLine = false;

        bool drawFeild = false;
        int mainColor = TEAM_NULL;

        bool drawingPath = false;
        autonPath path;

        Plot(const char* plotId = "", const char* plotLabel = "", int plotX = 0, int plotY = 0, int plotWidth = 0, int plotHeight = 0, int plotMaxX = 0, int plotMaxY = 0, int plotSubdiv = 0, bool showLabels = false, int teamColor = NAN, MenuSystem* menuSystemPointer = nullptr) {
            this->id = plotId;
            this->label = plotLabel;
            this->x = plotX;
            this->y = plotY;
            this->width = plotWidth;
            this->height = plotHeight;
            this->maxX = plotMaxX;
            this->maxY = plotMaxY;
            this->subdiv = plotSubdiv;
            this->labels = showLabels;
            this->drawFeild = !isnan(teamColor);
            if (drawFeild) {
                this->mainColor = teamColor;
                this->systemPtr = menuSystemPointer;
            }
        }

        void drawGameElements() {

            // Draw the Over Under Feild Elements

            vex::color ourColor;
            vex::color altColor;

            if (botAI.teamColor == TEAM_BLUE) {
                ourColor = color::blue;
                altColor = color::red;
            } else {
                ourColor = color::red;
                altColor = color::blue;
            }

            Brain.Screen.setPenWidth(2);

            // Left Goal
            Brain.Screen.setFillColor(transparent);
            Brain.Screen.setPenColor(altColor);
            Brain.Screen.drawRectangle(x + 1, y + 5 + height/3, width/6, height/3);

            // Right Goal
            Brain.Screen.setPenColor(ourColor);
            Brain.Screen.drawRectangle(x + 5*(width/6) + 1, y + 5 + height/3, width/6, height/3);

            // Our Matchloads
            Brain.Screen.setPenColor(ourColor);
            Brain.Screen.drawLine(x + width/6, y + 5, x, y + 5 + height/6);
            Brain.Screen.drawLine(x, y + 5 + 5*(height/6), x + width/6, y + 5 + height);

            // Alt Matchloads
            Brain.Screen.setPenColor(altColor);
            Brain.Screen.drawLine(x + 5*(width/6), y + 5, x + width, y + 5 + height/6);
            Brain.Screen.drawLine(x + width, y + 5 + 5*(height/6), x + 5*(width/6), y + 5 + height);

            // Middle Section
            Brain.Screen.setPenColor(ourColor);
            Brain.Screen.drawLine(x + width/2, y + 5 + 5*(height/6), x + width/2, y + 5 + height);
            Brain.Screen.setPenColor(altColor);
            Brain.Screen.drawLine(x + width/2, y + 5, x + width/2, y + 5 + height/6);

            Brain.Screen.setPenColor(color::white);
            Brain.Screen.drawLine(x + width/2, y + 5 + height/6, x + width/2, y + 5 + 5*(height/6));
            Brain.Screen.drawLine(x + width/3, y + 5 + height/6, x + 2*(width/3), y + 5 + height/6);
            Brain.Screen.drawLine(x + width/3, y + 5 + 5*(height/6), x + 2*(width/3), y + 5 + 5*(height/6));


            Brain.Screen.setPenWidth(1);
            Brain.Screen.setPenColor(color::white);
        }

        void draw() {
            Brain.Screen.printAt(x, y, label);
            if (subdiv != 0) {
                int moveX = width / subdiv;
                int moveY = height / subdiv;

                vex::color gray;
                gray.rgb(150, 150, 150);
                Brain.Screen.setPenColor(gray);
                
                // Vertical Lines
                for (int i = 0; i < subdiv; i++) {
                    if (labels) {
                        Brain.Screen.printAt(x + moveX*i, y + 3 + height, "%d", i);
                    }
                    Brain.Screen.drawLine(x + moveX*i, y + 5, x + moveX*i, y + 5 + height);
                }

                // Horizontal Lines
                for (int i = 1; i < subdiv; i++) {
                    if (labels) {
                        Brain.Screen.printAt(x, y + 3 + moveY*i, "%d", subdiv - i);
                    }
                    Brain.Screen.drawLine(x, y + 5 + moveY*i, x + width, y + 5 + moveY*i);
                }
                Brain.Screen.setPenColor(color::white);
            }


            if (drawFeild) {
                drawGameElements();
            }

            Brain.Screen.setFillColor(vex::color::transparent);
            Brain.Screen.drawRectangle(x, y + 5, width, height);
            Brain.Screen.setFillColor(black);

            if (drawStoredLine) { 
                drawLine(point1, point2);
            }
            if (drawingPath) {
                int lineWidth = 3;

                Position currentPos = path.startPos;
                int i = 0;
                while (true) {
                    autonMovement move = path.getStep(i);
                    if (move.movementType == AUTON_END) { break; }
                    Position newPos;
                    switch (move.movementType) {
                        case AUTON_DELAY:
                            
                            break;
                        case AUTON_DRIVE_DIST:
                            newPos = posAtDist(currentPos, move.val);
                            drawLine(currentPos, newPos, lineWidth);
                            currentPos = newPos;
                            break;
                        case AUTON_GOTO:
                            if (move.tilePosBool) {
                                newPos = path.pointer->odometrySystemPointer->tilePosToPos(move.tilePos);
                                drawLine(currentPos, newPos, lineWidth);
                                currentPos = newPos;
                            } else {
                                drawLine(currentPos, move.pos, lineWidth);
                                currentPos = move.pos;                                
                            }
                            break;
                        case AUTON_LONGGOTO:
                            for (int i = 0; i < move.drivePath.size(); i++) {
                                newPos = path.pointer->odometrySystemPointer->tilePosToPos(move.drivePath.at(i));
                                drawLine(currentPos, newPos, lineWidth);
                                currentPos = newPos;
                            }
                            break;
                        case AUTON_TURNTO:
                            //return pointer->turnTo(move.pos.rot);
                            break;
                        case AUTON_PICKUP_ACORN:
                            //return pointer->pickupAcorn();
                            break;
                        case AUTON_DROPOFF_ACORN:
                            //return pointer->dropAcorn();
                            break;
                    }
                    i++;
                    

                }
                    
            } else {
                drawPoints();
            }
            
        };
        void drawLine(Position pos1, Position pos2, int penWidth = 1) {
            int drawX1 = (( pos1.x / maxX ) * width) + x;
            int drawY1 = y + 5 + height - (( pos1.y / maxY) * height);


            int drawX2 = (( pos2.x / maxX ) * width) + x;
            int drawY2 = y + 5 + height - (( pos2.y / maxY) * height);

            Brain.Screen.setPenWidth(penWidth);
            Brain.Screen.setPenColor(cyan);
            Brain.Screen.drawLine(drawX1, drawY1, drawX2, drawY2);
            Brain.Screen.setPenColor(color::white);
            Brain.Screen.setPenWidth(1);
        }
        void updatePoint(int pointNum, bool draw, Position newPoint = Position()) {
            if (pointNum == 1) {
                point1 = newPoint;
                drawPoint1 = draw;
            }
            if (pointNum == 2) {
                point2 = newPoint;
                drawPoint2 = draw;
            }
        }

        void updateLine (bool draw) {
            drawStoredLine = draw;
        }
        void drawPoint(Position drawPoint, vex::color displayColor) {
            double radius = 5;

            int drawX = (( drawPoint.x / maxX ) * width) + x;
            int drawY = y + 5 + height - (( drawPoint.y / maxY) * height);

            Brain.Screen.setFillColor(displayColor);
            Brain.Screen.drawCircle(drawX, drawY, radius);
            Brain.Screen.setFillColor(black);
        }
        void drawPoints() {

            if (drawPoint2) {
                drawPoint(point2, cyan);
            }
            if (drawPoint1) {
                drawPoint(point1, color::green);
            }
        };

        void showPath(autonPath displayPath) {
            path = displayPath;
            drawingPath = true;
        }
};

int adjustingNumber(void* pagePointer);

class Page {
    private:

        Logger* loggerStorage;
        bool hasLogger = false;

        OverlayQuestion storedOverlay;
        bool showOverlay = false;

        std::vector<DisplayText> textStorage;
        std::vector<ProgressBar> barStorage;
        std::vector<Graph> graphStorage;
        std::vector<Button> buttonStorage;
        std::vector<DisplayBox> displayBoxStorage;
        std::vector<Toggle> toggleStorage;
        std::vector<Plot> plotStorage;

        int activeAdjustNum = NAN;
        std::vector<AdjustableNum> adjustNumStorage;
        



        int (*dataUpdaterCB)(Page*);
        double updateSpeed = 0.00;

        int (*pageLoadedCB)(Page*);
        bool hasLoadedCB = false;


        vex::color determinColorFromRange(int value, std::vector<colorRange> ranges) {
            for (auto range: ranges) {
                if (range.inRange(value)) {
                    return range.displayColor;
                }  
            }
            return vex::color::black;
        }
        
        bool inRectangle(int posX, int posY, int x, int y, int width, int height) {
            return (posX > x && posX < x + width) && (posY > y && posY < y + height);
        };


        void drawHorzProgressbar(ProgressBar bar) {
            Brain.Screen.printAt(bar.x, bar.y, bar.name, int(bar.value));
            Brain.Screen.drawRectangle(bar.x, bar.y + 5, bar.width, bar.height);

            if (bar.ranges.size() != 0) {
                Brain.Screen.setFillColor(determinColorFromRange(bar.value, bar.ranges));
            } else {
                // Default Battery Color Range
                std::vector<colorRange> defaultRanges;
                defaultRanges.push_back(colorRange(0, 20, color::red));
                defaultRanges.push_back(colorRange(21, 35, color::yellow));
                defaultRanges.push_back(colorRange(36, 100, color::green));

                Brain.Screen.setFillColor(determinColorFromRange(fabs(bar.value), defaultRanges));
            }
            

            if (bar.middle) {
                Brain.Screen.drawRectangle(bar.x + (bar.width / 2), bar.y + 5, (bar.value / 100 ) * (bar.width / 2), bar.height); // Middle Draw
            } else {
                Brain.Screen.drawRectangle(bar.x, bar.y + 5, (bar.value / 100) * bar.width, bar.height); // Normal Draw
            }
            Brain.Screen.setFillColor(vex::color::black);
        };
        void drawVertProgressBar(ProgressBar bar) {
            Brain.Screen.printAt(bar.x, bar.y, bar.name, int(bar.value));
            Brain.Screen.drawRectangle(bar.x, bar.y + 5, bar.width, bar.height);
            
            if (bar.ranges.size() != 0) {
                Brain.Screen.setFillColor(determinColorFromRange(bar.value, bar.ranges));
            } else {
                // Default Heat Color Range
                std::vector<colorRange> defaultRanges;
                defaultRanges.push_back(colorRange(0, 60, color::green));
                defaultRanges.push_back(colorRange(61, 80, color::yellow));
                defaultRanges.push_back(colorRange(81, 100, color::red));

                Brain.Screen.setFillColor(determinColorFromRange(fabs(bar.value), defaultRanges));
            }


            if (bar.middle) {
                Brain.Screen.drawRectangle(bar.x, bar.y + 5 + (bar.height / 2), bar.width, - (bar.value / 100) * bar.height / 2);
            } else {
                Brain.Screen.drawRectangle(bar.x, bar.y + 5 + bar.height, bar.width, - (bar.value / 100) * bar.height);
            }
    
            Brain.Screen.setFillColor(vex::color::black);
        };
        void drawButton(Button button) {
            Brain.Screen.setFillColor(button.fillColor);
            Brain.Screen.drawRectangle(button.x, button.y, button.width, button.height);
            Brain.Screen.setFillColor(black);
            Brain.Screen.printAt(button.x + (button.width / 2) - (Brain.Screen.getStringWidth(button.text) / 2), button.y + (button.height / 2) + (Brain.Screen.getStringHeight(button.text) / 3), button.text, button.data);
        };
        void drawDisplayBox(DisplayBox box) {
            Brain.Screen.setPenColor(box.penColor);
            Brain.Screen.setFillColor(box.fillColor);
            Brain.Screen.drawRectangle(box.x, box.y, box.width, box.height);
            Brain.Screen.setPenColor(color::white);
            Brain.Screen.setFillColor(black);            
        }
        void drawToggle(Toggle toggleButton) {

            if (toggleButton.status) {
                Brain.Screen.setFillColor(toggleButton.onColor);
            } else {
                Brain.Screen.setFillColor(toggleButton.offColor);
            }

            Brain.Screen.drawRectangle(toggleButton.x, toggleButton.y, toggleButton.width, toggleButton.height);
            if (toggleButton.showAlt) {
                Brain.Screen.printAt(toggleButton.x + (toggleButton.width / 2) - (Brain.Screen.getStringWidth(toggleButton.status ? toggleButton.trueText : toggleButton.displayText) / 2), toggleButton.y + (toggleButton.height / 2) + (Brain.Screen.getStringHeight(toggleButton.displayText) / 4), toggleButton.status ? toggleButton.trueText : toggleButton.displayText);
            } else {
                Brain.Screen.printAt(toggleButton.x + (toggleButton.width / 2) - (Brain.Screen.getStringWidth(toggleButton.displayText) / 2), toggleButton.y + (toggleButton.height / 2) + (Brain.Screen.getStringHeight(toggleButton.displayText) / 4), toggleButton.displayText);
            }
            Brain.Screen.setFillColor(black);
        }

        int screenXSize = 480;
        int screenYSize = 240;

        int overlayWidth = 300;
        int overlayHeight = 150;

        void drawOverlay(OverlayQuestion overlay) {


            int option1XCenter = (screenXSize / 2) - (overlayWidth / 4);
            int option2XCenter = (screenXSize / 2) + (overlayWidth / 4);

            Brain.Screen.drawRectangle((screenXSize / 2) - (overlayWidth / 2), (screenYSize / 2) - (overlayHeight / 2), overlayWidth, overlayHeight);

            //Brain.Screen.setFont(fontType::mono20);
            Brain.Screen.printAt((screenXSize / 2) - (Brain.Screen.getStringWidth(overlay.question.c_str()) / 2), (screenYSize / 2) - (overlayHeight / 2) + 40, overlay.question.c_str());
            Brain.Screen.setFont(fontType::mono30);

            Brain.Screen.setPenColor(overlay.option1Color); 
            Brain.Screen.printAt(option1XCenter - (Brain.Screen.getStringWidth(overlay.option1) / 2), (screenYSize / 2) + (overlayHeight / 2) - 25, overlay.option1);
        
            Brain.Screen.setPenColor(overlay.option2Color);
            Brain.Screen.printAt(option2XCenter - (Brain.Screen.getStringWidth(overlay.option2) / 2), (screenYSize / 2) + (overlayHeight / 2) - 25, overlay.option2);

            Brain.Screen.setPenColor(color::white);
            Brain.Screen.setFont(fontType::mono20);


        };


    public:

        bool hasCB = false;
        MenuSystem* menuSystemPointer = nullptr; // So then the page can access the MenuSystem controlling it

        bool pageChanged = false;

        Page() {};

        void render() {

            // Render Logger
            if (hasLogger) {
                loggerStorage->render();
            }

            // Render Texts

            for (auto text: textStorage) {
                Brain.Screen.setPenColor(text.displayColor);
                Brain.Screen.setFont(text.displayFont);
                if (!text.displayInt) {
                    Brain.Screen.printAt(text.x, text.y, text.text, text.displayDoubleData);
                } else {
                    Brain.Screen.printAt(text.x, text.y, text.text, text.displayData);
                }
                Brain.Screen.setPenColor(color::white);
                Brain.Screen.setFont(fontType::mono20);
            }

            // Render Display Boxes
            for (auto displayBox: displayBoxStorage) {
                drawDisplayBox(displayBox);
            }

            // Render Progress Bars
            for (auto bar: barStorage) {
                if (bar.vertical) {
                    drawVertProgressBar(bar);
                } else {
                    drawHorzProgressbar(bar);
                }
            }

            // Render Line Graphs
            for (auto graph: graphStorage) {
                graph.draw();
            }

            // Render Buttons
            for (auto button: buttonStorage) {
                drawButton(button);
            }

            // Render Toggles
            for (auto toggle: toggleStorage) {
                drawToggle(toggle);
            }

            // Render Plots
            for (auto &plot: plotStorage) {
                plot.draw();
            }

            // Render Adjustable Numers
            for (auto adjustNum: adjustNumStorage) {
                adjustNum.render();
            }

            // Render Overlay
            if (showOverlay) {
                drawOverlay(storedOverlay);
            }

        };

        void addDataUpdaterCB(int (*cb)(Page*), double refreshRate = 1) {
            dataUpdaterCB = cb;
            hasCB = true;
            updateSpeed = refreshRate;
        };
        void addPageLoadedCB(int (*cb)(Page*)) {
            pageLoadedCB = cb;
            hasLoadedCB = true;
        };


        bool updaterRunning = false;
        int updateData() {
            updaterRunning = true;
            while (updaterRunning) {
                dataUpdaterCB(this);
                wait(updateSpeed, seconds);                
            }
            return 1;
        };
        void stopUpdater() {
            updaterRunning = false;
        }
        void pageLoaded() {
            if (!hasLoadedCB) { return; };
            pageLoadedCB(this);
        };


        void addLogger(Logger* loggerMemLocation) {
            loggerStorage = loggerMemLocation;
            hasLogger = true;
        };
        void addHorzProgressBar(const char* barId, int x, int y, int width, int height, const char* name, bool middle, std::vector<colorRange> ranges) {
            barStorage.push_back(ProgressBar(barId, x, y, width, height, name, false, middle, ranges));
        };
        void addVertProgressBar(const char* barId, int x, int y, int width, int height, const char* name, bool middle, std::vector<colorRange> ranges) {
            barStorage.push_back(ProgressBar(barId, x, y, width, height, name, true, middle, ranges));
        };
        void addLineGraph(const char* graphId, const char* graphName, int xPos, int yPos, int graphWidth, int graphHeight, bool drawMiddle, std::vector<colorRange> colorRanges, int maxDataPoints = 10) {
            graphStorage.push_back(Graph());
            graphStorage.back().build(graphId, graphName, xPos, yPos, graphWidth, graphHeight, drawMiddle, colorRanges, maxDataPoints);
        };
        void addText(const char* text, int x, int y, vex::color displayColor = color::white, vex::fontType displayFont = fontType::mono20, const char* id = "") {
            textStorage.push_back(DisplayText(text, x, y, displayColor, displayFont, id));
        }
        void addButton(const char* text, int x, int y, int width, int height, int (*cb)(Page*), const char* id = "", vex::color fillColor = black) {
            buttonStorage.push_back(Button(text, x, y, width, height, cb, id, fillColor));
        };
        void addDisplayBox(const char* id, int x, int y, int width, int height, vex::color fillColor, vex::color penColor = color::white) {
            displayBoxStorage.push_back(DisplayBox(id, x, y, width, height, fillColor, penColor));
        }
        void addToggle(const char* id, const char* displayText, bool startStatus, vex::color offColor, vex::color onColor, int x, int y, int width, int height, const char* trueText = "") {
            toggleStorage.push_back(Toggle(id, displayText, startStatus, offColor, onColor, x, y, width, height, trueText));
        };
        void addPlot(const char* plotId, const char* label, int plotX, int plotY, int plotWidth, int plotHeight, int plotMaxX, int plotMaxY, int plotSubdiv = 0, bool showLabels = false, int teamColor = NAN) {
            plotStorage.push_back(Plot(plotId, label, plotX, plotY, plotWidth, plotHeight, plotMaxX, plotMaxY, plotSubdiv, showLabels, teamColor, menuSystemPointer));
        };
        void addAdjustableNum(const char* id, double initialVal, double stepAmount, double maxAmount, double minAmount, int displayX, int displayY, int displayWidth, int displayHeight, vex::fontType displayFont, bool showRanges) {
            adjustNumStorage.push_back(AdjustableNum(id, initialVal, stepAmount, maxAmount, minAmount, displayX, displayY, displayWidth, displayHeight, displayFont, this, showRanges));
        };



        void setProgressBarValue(const char* barId, int value) {
            for (auto &bar: barStorage) {
                if (strcmp(barId, bar.id) == 0) {
                    bar.value = value;
                    return;
                }
            }
        };
        void setLineGraphValue(const char* graphId, int value) {
            for (auto &graph: graphStorage) {
                if (strcmp(graphId, graph.getId()) == 0) {
                    graph.addPoint(value);
                    return;
                }
            }
        };
        
        void setTextData(const char* textId, vex::color displayColor) {
            for (auto &text: textStorage) {
                if (strcmp(text.id, textId) == 0) {
                    if (displayColor != NAN) {
                        text.displayColor = displayColor;
                    }
                    return;
                }
            }
        };
        void setTextData(const char* textId, vex::color displayColor, const char* newText) {
            for (auto &text: textStorage) {
                if (strcmp(text.id, textId) == 0) {
                    if (displayColor != NAN) {
                        text.displayColor = displayColor;
                    }
                    if (strcmp(newText, "") != 0) {
                        text.text = newText;
                    }
                    return;
                }
            }
        };
        void setTextData(const char* textId, int data) {
            for (auto &text: textStorage) {
                if (strcmp(text.id, textId) == 0) {
                    if (data != NAN) {
                        text.displayData = data;
                    }
                    text.displayDoubleData = NAN;
                    text.displayInt = true;
                    return;
                }
            }
        };
        void setTextData(const char* textId, double doubleData) {
            for (auto &text: textStorage) {
                if (strcmp(text.id, textId) == 0) {
                    text.displayData = NAN;
                    if (doubleData != NAN) {
                        text.displayDoubleData = doubleData;
                    }
                    text.displayInt = false;
                    return;
                }
            }
        };
        
        void setButtonData(const char* buttonId, vex::color fillColor, int data = 0) {
            for (auto &button: buttonStorage) {
                if (strcmp(button.text, buttonId) == 0) {
                    button.data = data;
                    button.fillColor = fillColor;
                    return;
                }
            }
        }
        void setDisplayBoxData(const char* boxId, vex::color fillColor, vex::color penColor = color::white) {
            for (auto &displayBox: displayBoxStorage) {
                if (strcmp(displayBox.id, boxId) == 0) {
                    displayBox.fillColor = fillColor;
                    displayBox.penColor = penColor;
                    return;
                }
            }
        };
        void setToggleStatus(const char* toggleId, bool status) {
            for (auto &toggle: toggleStorage) {
                if (strcmp(toggle.id, toggleId) == 0) {
                    toggle.status = status;
                    return;
                }
            }
        };
        Plot* getPlotPointer(const char* id) {
            for (auto &plot: plotStorage) {
                if (strcmp(plot.id, id) == 0) {
                    return &plot;
                }
            }
            return nullptr;
        }




        void massSetData(const char* id, int data) {
            setProgressBarValue(id, data);
            setLineGraphValue(id, data);
        };


        bool getToggleStatus(const char* toggleId) {
            for (auto toggle: toggleStorage) {
                if (strcmp(toggle.id, toggleId) == 0) {
                    return toggle.status;
                }
            }
            return false;
        }

        AdjustableNum* getCurrentAdjustNum() {
            if (isnan(activeAdjustNum)) {return nullptr;}
            return &adjustNumStorage[activeAdjustNum];
        }
        double getAdjustNum(const char* id) {
            for (auto num: adjustNumStorage) {
                if (strcmp(num.getId(), id) == 0) {
                    return num.getVal();
                }
            }
        }

        // Returns false for option1 and true for option2
        bool overlayQuestion(OverlayQuestion overlay) {
            storedOverlay = overlay;
            showOverlay = true;

            int lastX = Brain.Screen.xPosition();
            int lastY = Brain.Screen.yPosition();

            while (true) {

                int clickX = Brain.Screen.xPosition();
                int clickY = Brain.Screen.yPosition();

                if (clickX != lastX && clickY != lastY) {
                    if (inRectangle(clickX, clickY, (screenXSize / 2) - (overlayWidth / 2), (screenYSize / 2), overlayWidth / 2, overlayHeight / 2)) {
                        showOverlay = false;
                        return false;
                    } 
                    if (inRectangle(clickX, clickY, screenXSize / 2, screenYSize / 2, overlayWidth / 2, overlayHeight / 2)) {
                        showOverlay = false;
                        return true;
                    };
                }
                wait(0.1, seconds);

            };
        };

        // Returns -1 for option 1, 1 for option 2, 0 for off screen, 10 for the box
        int advOverlayQuestion(OverlayQuestion overlay) {
            storedOverlay = overlay;
            showOverlay = true;

            int lastX = Brain.Screen.xPosition();
            int lastY = Brain.Screen.yPosition();

            while (true) {

                int clickX = Brain.Screen.xPosition();
                int clickY = Brain.Screen.yPosition();

                if (clickX != lastX && clickY != lastY) {
                    if (inRectangle(clickX, clickY, (screenXSize / 2) - (overlayWidth / 2), (screenYSize / 2), overlayWidth / 2, overlayHeight / 2)) {
                        showOverlay = false;
                        return -1;
                    } 
                    if (inRectangle(clickX, clickY, screenXSize / 2, screenYSize / 2, overlayWidth / 2, overlayHeight / 2)) {
                        showOverlay = false;
                        return 1;
                    };
                    if (inRectangle(clickX, clickY, (screenXSize / 2) - (overlayWidth / 2), (screenYSize / 2) - (overlayHeight / 2), overlayWidth, overlayHeight)) {
                        showOverlay = false;
                        return 10;
                    }
                    showOverlay = false;
                    return 0;
                }
                wait(0.1, seconds);

            };
        }



        void screenPressed(int x, int y) {
            // Check for buttons
            for (auto &button: buttonStorage) {
                if (inRectangle(x, y, button.x, button.y, button.width, button.height)) {
                    button.cb(this);
                    return;
                }
            }

            // Check for toggles
            for (auto &toggle: toggleStorage) {
                if (inRectangle(x, y, toggle.x, toggle.y, toggle.width, toggle.height)) {
                    pageChanged = true;
                    if (toggle.status) {
                        toggle.status = false;
                    } else {
                        toggle.status = true;
                    }
                }
            }

            // Check Adjustable Numbers
            int i = 0;
            for (auto &num: adjustNumStorage) {
                if (num.checkPressed(x, y)) {
                    activeAdjustNum = i;
                    task(adjustingNumber, (void*)this, task::taskPriorityNormal);
                    return;
                }
                i++;
            }
        }

};

// The Adjusting Number Task
int adjustingNumber(void* pagePointer) {
    wait(0.1, seconds);
    Page* ptr = (Page*)pagePointer;
    bool running = true;
    while (running) {
        AdjustableNum* adjustPtr = ptr->getCurrentAdjustNum();

        OverlayQuestion overlay;
        overlay.option1 = "V";
        overlay.option2 = "^";
        overlay.option1Color = color::green;
        overlay.option2Color = color::green;
        overlay.question = adjustPtr->getDisplayVal().c_str();
        switch (ptr->advOverlayQuestion(overlay))
        {
        case 1:
            adjustPtr->increase();
            break;

        case -1:
            adjustPtr->decrease();
            break;
        
        case 10:
            break;

        case 0:
            running = false;
            break;
        }
    }

    return 1;
};



struct Notification {
    const char* text;
    vex::color displayColor = color::white;
    double disapearTime;
    Notification(const char* displayText, int time, vex::color showColor = color::white) {
        text = displayText;
        displayColor = showColor;
        disapearTime = time;
    };
    Notification();
};

// I would put this in the MenuSystem Class but I can't call it as a task() in the MenuSystem Class
int mainDataUpdater(void* pageToUpdate) {
    Page* pagePointer = (Page*)pageToUpdate;
    pagePointer->updateData();
    return 1;
}

class MenuSystem {
    private:

        Page* pageStorage[10];
        const char* pageIdStorage[10];
        int pagesStored = 0;

        int displayPage = -1;
        int prevPage = 0;

        bool firstTimeRender = true;

        task updaterTask;
        bool isUpdating = false;

        void startUpdaterTask(int pageNum) {
            if (isUpdating) { stopUpdaterTask(); }
            if (!pageStorage[displayPage]->hasCB) { return; }
            updaterTask = task(mainDataUpdater, (void*)pageStorage[displayPage], task::taskPrioritylow);
            isUpdating = true;
        }; 
        void stopUpdaterTask() {
            updaterTask.stop(updaterTask);
            isUpdating = false;
            //brainDebug("Ended Task");
        };


        task notificationTask;
        std::vector<Notification> notifications;
        bool showingNotifications = false;

        void drawNotification(int rowNum, Notification notif) {
            int screenXSize = 480;
            //int width = 100;
            int width = Brain.Screen.getStringWidth(notif.text) + 20;
            //int height = 40;
            int height = Brain.Screen.getStringHeight(notif.text) + 10;

            Brain.Screen.drawRectangle(screenXSize - width, (rowNum * height), width, height);
            Brain.Screen.setPenColor(notif.displayColor);
            Brain.Screen.printAt(screenXSize - (width / 2) - (Brain.Screen.getStringWidth(notif.text) / 2), (rowNum * height) + (height / 2) + (Brain.Screen.getStringHeight(notif.text) / 4), notif.text);
            Brain.Screen.setPenColor(color::white);
        }

    public:

        int teamColor = 0;

        MenuSystem(bool displayNotifications) {
            showingNotifications = displayNotifications;
        };
      
        void render() {
            // Render Page
            if (displayPage == -1) {
                Brain.Screen.printAt(20, 100, "No Pages In Storage");
            } else {
                pageStorage[displayPage]->render();
                if (firstTimeRender) { startUpdaterTask(displayPage); firstTimeRender = false;}
            }

            

            // Render Notifications
            if (showingNotifications) {
                for (int i = notifications.size() - 1; i > -1; i--) {
                    if (notifications[i].disapearTime < Brain.timer(msec)) {
                        notifications.erase(notifications.begin() + i);
                    } else {
                        if (displayPage == 0) { return; } // Don't render notifications on the loading page
                        drawNotification(i, notifications.at(i));
                    }
                    
                }
            }
        };

        void addPage(const char* pageId, Page* page) {
            pageStorage[pagesStored] = page;
            pageStorage[pagesStored]->menuSystemPointer = this;
            pageIdStorage[pagesStored] = pageId;
            pagesStored++;
            if (displayPage == -1 && pagesStored == 1) {
                displayPage = 0;
            }
        };

        void gotoPage(const char* pageId) {
            for (int i = 0; i < pagesStored; i++) {
                if (strcmp(pageId, pageIdStorage[i]) == 0) {
                    if (displayPage != i) {
                        prevPage = displayPage;
                        displayPage = i;
                        startUpdaterTask(displayPage);
                        pageStorage[i]->pageLoaded();
                        return;
                    } else { return; }
                }
            }
        };

        void gotoPrevPage() {
            gotoPage(pageIdStorage[prevPage]);
        }

        Page* searchPages(const char* pageId) {
            for (int i = 0; i < pagesStored; i++) {
                if (strcmp(pageId, pageIdStorage[i]) == 0) {
                    return pageStorage[i];
                }
            }
            return nullptr;
        }
        
        void screenPressed() {
            if (displayPage != 0) {
                pageStorage[displayPage]->screenPressed(Brain.Screen.xPosition(), Brain.Screen.yPosition());
            } else {
                gotoPage("main");
            }
        };



        void newNotification(const char* text, int displayTime, vex::color displayColor = color::white) {
            if (!showingNotifications) {return;}
            notifications.push_back(Notification(text, Brain.timer(msec) + (1000.00 * displayTime), displayColor));
        };

};



struct NotCheck {
    const char* trueMessage;
    vex::color trueColor = color::green;
    const char* falseMessage;
    vex::color falseColor = color::red;
    bool log;
    bool lastVal;
    bool (*checkCB)();
    NotCheck(const char* trueMessage, const char* falseMessage, bool (*cb)(), bool initVal = false, vex::color trueColor = color::green, vex::color falseColor = color::red, bool log = false) {
        this->trueMessage = trueMessage;
        this->falseMessage = falseMessage;
        this->checkCB = cb;
        this->lastVal = initVal;
        this->trueColor = trueColor;
        this->falseColor;
        this->log = log;
    }
};

bool notMotorCheck(void* motorToCheck) {
    motor* motorPointer = (motor*)motorToCheck;
    return motorPointer->temperature(temperatureUnits::fahrenheit) >= motorWarnTemp;
}

struct MotCheck {
    const char* motorName;
    motor* motorPointer;
    int warnTimeout;
    double lastWarn = 0; 
    MotCheck(const char* motorName, motor* motorPtr, int warnTime = 30) {
        this->motorName = motorName;
        this->motorPointer = motorPtr;
        this->warnTimeout = warnTime;
    }
};

class NotificationChecker {
    private:
        std::vector<NotCheck> checksStorage;
        std::vector<MotCheck> motorCheckStorage;
        

        MenuSystem* menuSystemPtr;

        bool checkMotor(MotCheck check) {
            return check.motorPointer->temperature(temperatureUnits::fahrenheit) >= motorWarnTemp;
        }

    public:
        NotificationChecker(MenuSystem* ptr) {
            menuSystemPtr = ptr;
        };

        void addCheck(const char* trueMessage, const char* falseMessage, bool (*cb)(), bool initVal = false, vex::color trueColor = color::green, vex::color falseColor = color::red, bool log = false) {
            checksStorage.push_back(NotCheck(trueMessage, falseMessage, cb, initVal, trueColor, falseColor, log));
        }
        void addMotor(const char* motorName, motor* motorPtr, int warnTime = 30) {
            motorCheckStorage.push_back(MotCheck(motorName, motorPtr, warnTime));
        }
        void check() {
            for (auto &check: checksStorage) {
                bool newVal = check.checkCB();
                if (newVal != check.lastVal) {
                    if (check.log) {
                        brainFancyDebug(newVal ? check.trueMessage : check.falseMessage, newVal ? check.trueColor : check.falseColor, true);
                    } else {
                        menuSystemPtr->newNotification(newVal ? check.trueMessage : check.falseMessage, 4, newVal ? check.trueColor : check.falseColor);
                    } 
                }
                check.lastVal = newVal;
            }
            for (auto &check: motorCheckStorage) {
                if (checkMotor(check) && check.lastWarn < Brain.timer(timeUnits::msec)) {
                    
                    std::stringstream tmpString;
                    tmpString << check.motorName << " Overheated";

                    brainError(tmpString.str().c_str(), true);

                    check.lastWarn = Brain.timer(timeUnits::msec) + (check.warnTimeout * 1000);
                }
            }
        }
};