#include "vex.h"
#include "string"
#include "robotConfig.h"


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

        int xPos;
        int yPos;

        int maxRows;
        int currentLine;

        const char* logFile;

        LogMessage brainLog[11];

        void brainLogShift() {
            for (int i=0; i < maxRows - 1; i++) {
                brainLog[i] = brainLog[i + 1];
            }
        };

        void appendLogFile(const char* message) {
            if (!Brain.SDcard.isInserted()) { return; }

            std::ostringstream formattedMessage;
            formattedMessage << "[ " << Brain.timer(seconds) << " ] " << message;

            appendFile(logFile, formattedMessage.str().c_str());
        };

    public:
        Logger(int renderX, int renderY, const char* outFile = "logs.txt", int maxLogs = 11) {
            xPos = renderX;
            yPos = renderY;
            maxRows = maxLogs;

            logFile = outFile;

            writeFile(logFile, "Start of Logs");
        };

        void newLog(const char* message, vex::color messageColor, int data = 0) {
            if (currentLine == maxRows) {
                brainLogShift();
                currentLine = maxRows - 1;
            }

            brainLog[currentLine] = LogMessage(message, messageColor, data);
            currentLine ++;

            appendLogFile(message);
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
    colorRange ranges[205];
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
};

struct DisplayBox {
    int x;
    int y;
    int width;
    int height;
    const char* id;
    vex::color fillColor;
    vex::color penColor;
};

class Page; // Forward decloration so the button class can build
class MenuSystem;

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
};

struct OverlayQuestion {
    const char* question;    
    const char* option1;
    vex::color option1Color = white;    
    const char* option2;
    vex::color option2Color = white;
};

struct Toggle {
    const char* displayText;
    bool status;
    vex::color offColor;
    vex::color onColor;
    int x;
    int y;
    int width;
    int height;
    
};

class Plot {
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


        Position point1;
        bool drawPoint1 = false;
        Position point2;
        bool drawPoint2 = false;

        bool drawStoredLine = false;

        Plot(const char* plotId = "", const char* plotLabel = "", int plotX = 0, int plotY = 0, int plotWidth = 0, int plotHeight = 0, int plotMaxX = 0, int plotMaxY = 0, int plotSubdiv = 0) {
            id = plotId;
            label = plotLabel;
            width = plotWidth;
            height = plotHeight;
            maxX = plotMaxX;
            maxY = plotMaxY;
            subdiv = plotSubdiv;
        }
        
        void draw() {
            Brain.Screen.printAt(x, y, label);
            if (subdiv != 0) {
                int moveX = width / subdiv;
                int moveY = height / subdiv;

                vex::color gray;
                gray.rgb(150, 150, 150);
                Brain.Screen.setPenColor(gray);
                for (int i = 0; i < subdiv; i++) {
                    Brain.Screen.drawLine(x + moveX*i, y + 5, x + moveX*i, y + 5 + height);
                }

                for (int i = 0; i < subdiv; i++) {
                    Brain.Screen.drawLine(x, y + 5 + moveY*i, x + width, y + 5 + moveY*i);
                }
                Brain.Screen.setPenColor(white);
            }
            
            Brain.Screen.setFillColor(vex::color::transparent);
            Brain.Screen.drawRectangle(x, y + 5, width, height);
            Brain.Screen.setFillColor(black);

            if (drawStoredLine) { 
                int drawX1 = (( point1.x / maxX ) * width) + x;
                int drawY1 = y + 5 + height - (( point1.y / maxY) * height);


                int drawX2 = (( point2.x / maxX ) * width) + x;
                int drawY2 = y + 5 + height - (( point2.y / maxY) * height);

                Brain.Screen.setPenColor(cyan);
                Brain.Screen.drawLine(drawX1, drawY1, drawX2, drawY2);
                Brain.Screen.setPenColor(white);

                wait(0.1, seconds);

            }
            drawPoints();
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
                drawPoint(point1, green);
            }
        };
};

class Page {
    private:

        DisplayText textStorage[10];
        int textsStored = 0;

        ProgressBar barStorage[10];
        int barsStored = 0;

        Graph graphStorage[10];
        const char* graphIdStorage[10];
        int graphsStored = 0;

        Logger* loggerStorage;
        bool hasLogger = false;

        Button buttonStorage[10];
        int buttonsStored = 0;

        DisplayBox displayBoxStorage[10];
        int displayBoxsStored = 0;

        Toggle toggleStorage[10];
        int togglesStored = 0;

        Plot plotStorage[10];
        int plotsStored = 0;

        OverlayQuestion storedOverlay;
        bool showOverlay = false;


        int (*dataUpdaterCB)(Page*);
        double updateSpeed = 0.00;

        int (*pageLoadedCB)(Page*);
        bool hasLoadedCB = false;


        vex::color determinColorFromRange(int value, colorRange ranges[]) {
            for (int i = 0; !ranges[i].isDefault(); i++) {
                if (ranges[i].inRange(value)) {
                    return ranges[i].displayColor;
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
        void drawVertProgressBar(ProgressBar bar) {
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
            Brain.Screen.setPenColor(white);
            Brain.Screen.setFillColor(black);            
        }
        void drawToggle(Toggle toggleButton) {

            if (toggleButton.status) {
                Brain.Screen.setFillColor(toggleButton.onColor);
            } else {
                Brain.Screen.setFillColor(toggleButton.offColor);
            }

            Brain.Screen.drawRectangle(toggleButton.x, toggleButton.y, toggleButton.width, toggleButton.height);
            Brain.Screen.printAt(toggleButton.x + (toggleButton.width / 2) - (Brain.Screen.getStringWidth(toggleButton.displayText) / 2), toggleButton.y + (toggleButton.height / 2) + (Brain.Screen.getStringHeight(toggleButton.displayText) / 4), toggleButton.displayText);
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
            Brain.Screen.printAt((screenXSize / 2) - (Brain.Screen.getStringWidth(overlay.question) / 2), (screenYSize / 2) - (overlayHeight / 2) + 40, overlay.question);
            Brain.Screen.setFont(fontType::mono30);

            Brain.Screen.setPenColor(overlay.option1Color); 
            Brain.Screen.printAt(option1XCenter - (Brain.Screen.getStringWidth(overlay.option1) / 2), (screenYSize / 2) + (overlayHeight / 2) - 25, overlay.option1);
        
            Brain.Screen.setPenColor(overlay.option2Color);
            Brain.Screen.printAt(option2XCenter - (Brain.Screen.getStringWidth(overlay.option2) / 2), (screenYSize / 2) + (overlayHeight / 2) - 25, overlay.option2);

            Brain.Screen.setPenColor(white);
            Brain.Screen.setFont(fontType::mono20);


        };


    public:

        bool hasCB = false;
        MenuSystem* menuSystemPointer = nullptr; // So then the page can access the MenuSystem controlling it

        bool pageChanged = false;

        Page(int test) {};

        void render() {

            // Render Logger
            if (hasLogger) {
                loggerStorage->render();
            }

            // Render Texts
            for (int i = 0; i < textsStored; i++) {
                Brain.Screen.setPenColor(textStorage[i].displayColor);
                Brain.Screen.setFont(textStorage[i].displayFont);
                if (!textStorage[i].displayInt) {
                    Brain.Screen.printAt(textStorage[i].x, textStorage[i].y, textStorage[i].text, textStorage[i].displayDoubleData);
                } else {
                    Brain.Screen.printAt(textStorage[i].x, textStorage[i].y, textStorage[i].text, textStorage[i].displayData);
                }
                Brain.Screen.setPenColor(white);
                Brain.Screen.setFont(fontType::mono20);
            }

            // Render Display Boxes
            for (int i = 0; i < displayBoxsStored; i++) {
                drawDisplayBox(displayBoxStorage[i]);
            }

            // Render Progress Bars
            for (int i = 0; i < barsStored; i++) {
                if (barStorage[i].vertical) {
                    drawVertProgressBar(barStorage[i]);
                } else {
                    drawHorzProgressbar(barStorage[i]);
                }
            }

            // Render Line Graphs
            for (int i = 0; i < graphsStored; i++) {
                graphStorage[i].draw();
            }

            // Render Buttons
            for (int i = 0; i < buttonsStored; i++) {
                drawButton(buttonStorage[i]);
            }

            // Render Toggles
            for (int i = 0; i < togglesStored; i++) {
                drawToggle(toggleStorage[i]);
            }

            // Render Plots
            for (int i = 0; i < plotsStored; i++) {
                plotStorage[i].draw();
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


        int updateData() {
            while (true) {
                dataUpdaterCB(this);
                wait(updateSpeed, seconds);                
            }
            return 1;
        };
        void pageLoaded() {
            if (!hasLoadedCB) { return; };
            pageLoadedCB(this);
        };


        void addLogger(Logger* loggerMemLocation) {
            loggerStorage = loggerMemLocation;
            hasLogger = true;
        };
        void addHorzProgressBar(const char* barId, int x, int y, int width, int height, const char* name, bool middle = false, colorRange ranges[] = nullptr) {
            ProgressBar tempBar;
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
            ProgressBar tempBar;
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
        void addText(const char* text, int x, int y, vex::color displayColor = white, vex::fontType displayFont = fontType::mono20, const char* id = "") {
            textStorage[textsStored] = DisplayText();
            textStorage[textsStored].text = text;
            textStorage[textsStored].x = x;
            textStorage[textsStored].y = y;
            textStorage[textsStored].displayData = 0;
            textStorage[textsStored].displayColor = displayColor;
            textStorage[textsStored].displayFont = displayFont;
            textStorage[textsStored].id = id;
            textsStored++;
        }
        void addButton(const char* text, int x, int y, int width, int height, int (*cb)(Page*), const char* id = "", vex::color fillColor = black) {
            buttonStorage[buttonsStored] = Button();
            buttonStorage[buttonsStored].text = text;
            buttonStorage[buttonsStored].x = x;
            buttonStorage[buttonsStored].y = y;
            buttonStorage[buttonsStored].width = width;
            buttonStorage[buttonsStored].height = height;
            buttonStorage[buttonsStored].id = id;
            buttonStorage[buttonsStored].fillColor = fillColor;
            buttonStorage[buttonsStored].cb = cb;
            buttonsStored++;
        };
        void addDisplayBox(const char* id, int x, int y, int width, int height, vex::color fillColor, vex::color penColor = white) {
            displayBoxStorage[displayBoxsStored] = DisplayBox();
            displayBoxStorage[displayBoxsStored].x = x;
            displayBoxStorage[displayBoxsStored].y = y;
            displayBoxStorage[displayBoxsStored].width = width;
            displayBoxStorage[displayBoxsStored].height = height;
            displayBoxStorage[displayBoxsStored].id = id;
            displayBoxStorage[displayBoxsStored].fillColor = fillColor;
            displayBoxStorage[displayBoxsStored].penColor = penColor;
            displayBoxsStored++;
        }
        void addToggle( const char* displayText, bool startStatus, vex::color offColor, vex::color onColor, int x, int y, int width, int height) {
            toggleStorage[togglesStored] = Toggle();
            toggleStorage[togglesStored].displayText = displayText;
            toggleStorage[togglesStored].status = startStatus;
            toggleStorage[togglesStored].offColor = offColor;
            toggleStorage[togglesStored].onColor = onColor;
            toggleStorage[togglesStored].x = x;
            toggleStorage[togglesStored].y = y;
            toggleStorage[togglesStored].width = width;
            toggleStorage[togglesStored].height = height;
            togglesStored++;
        };
        void addPlot(const char* plotId, const char* label, int plotX, int plotY, int plotWidth, int plotHeight, int plotMaxX, int plotMaxY, int plotSubdiv = 0) {
            plotStorage[plotsStored] = Plot();
            plotStorage[plotsStored].id = plotId;
            plotStorage[plotsStored].label = label;
            plotStorage[plotsStored].x = plotX;
            plotStorage[plotsStored].y = plotY;
            plotStorage[plotsStored].width = plotWidth;
            plotStorage[plotsStored].height = plotHeight;
            plotStorage[plotsStored].maxX = plotMaxX;
            plotStorage[plotsStored].maxY = plotMaxY;
            plotStorage[plotsStored].subdiv = plotSubdiv;

            plotsStored++;
        };


        void setProgressBarValue(const char* barId, int value) {
            for (int i = 0; i < barsStored; i++) {
                if (strcmp(barId, barStorage[i].id) == 0) {
                    barStorage[i].value = value;
                    return;
                }
            }
        };
        void setLineGraphValue(const char* graphId, int value) {
            for (int i = 0; i < graphsStored; i++) {
                if (strcmp(graphId, graphIdStorage[i]) == 0) {
                    graphStorage[i].addPoint(value);
                    return;
                }
            }
        };
        
        void setTextData(const char* textId, vex::color displayColor) {
            for (int i = 0; i < textsStored; i++) {
                if (strcmp(textStorage[i].id, textId) == 0) {
                    if (displayColor != NAN) {
                        textStorage[i].displayColor = displayColor;
                    }
                    return;
                }
            }
        };
        void setTextData(const char* textId, vex::color displayColor, const char* newText) {
            for (int i = 0; i < textsStored; i++) {
                if (strcmp(textStorage[i].id, textId) == 0) {
                    if (displayColor != NAN) {
                        textStorage[i].displayColor = displayColor;
                    }
                    if (strcmp(newText, "") != 0) {
                        textStorage[i].text = newText;
                    }
                    return;
                }
            }
        };
        void setTextData(const char* textId, int data) {
            for (int i = 0; i < textsStored; i++) {
                if (strcmp(textStorage[i].id, textId) == 0) {
                    if (data != NAN) {
                        textStorage[i].displayData = data;
                    }
                    textStorage[i].displayDoubleData = NAN;
                    textStorage[i].displayInt = true;
                    return;
                }
            }
        };
        void setTextData(const char* textId, double doubleData) {
            for (int i = 0; i < textsStored; i++) {
                if (strcmp(textStorage[i].id, textId) == 0) {
                    textStorage[i].displayData = NAN;
                    if (doubleData != NAN) {
                        textStorage[i].displayDoubleData = doubleData;
                    }
                    textStorage[i].displayInt = false;
                    return;
                }
            }
        };
        
        void setButtonData(const char* buttonId, vex::color fillColor, int data = 0) {
            for (int i = 0; i < buttonsStored; i++) {
                if (strcmp(buttonStorage[i].text, buttonId) == 0) {
                    buttonStorage[i].data = data;
                    buttonStorage[i].fillColor = fillColor;
                    return;
                }
            }
        }
        void setDisplayBoxData(const char* boxId, vex::color fillColor, vex::color penColor = white) {
            for (int i = 0; i < displayBoxsStored; i++) {
                if (strcmp(displayBoxStorage[i].id, boxId) == 0) {
                    displayBoxStorage[i].fillColor = fillColor;
                    displayBoxStorage[i].penColor = penColor;
                    return;
                }
            }
        };
        void setToggleStatus(const char* toggleId, bool status) {
            for (int i = 0; i < togglesStored; i++) {
                if (strcmp(toggleStorage[i].displayText, toggleId) == 0) {
                    toggleStorage[i].status = status;
                    return;
                }
            }
        };
        Plot* getPlotPointer(const char* id) {
            for (int i = 0; i < plotsStored; i++) {
                if (strcmp(plotStorage[i].id, id) == 0) {
                    return &plotStorage[i];
                }
            }
            return nullptr;
        }





        void massSetData(const char* id, int data) {
            setProgressBarValue(id, data);
            setLineGraphValue(id, data);
        };


        bool getToggleStatus(const char* toggleId) {
            for (int i = 0; i < togglesStored; i++) {
                if (strcmp(toggleStorage[i].displayText, toggleId) == 0) {
                    return toggleStorage[i].status;
                }
            }
            return false;
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

        void screenPressed(int x, int y) {
            // Check for buttons
            for (int i = 0; i < buttonsStored; i++) {
                if (inRectangle(x, y, buttonStorage[i].x, buttonStorage[i].y, buttonStorage[i].width, buttonStorage[i].height)) {
                    buttonStorage[i].cb(this);
                    return;
                }
            }

            // Check for toggles
            for (int i = 0; i < togglesStored; i++) {
                if (inRectangle(x, y, toggleStorage[i].x, toggleStorage[i].y, toggleStorage[i].width, toggleStorage[i].height)) {
                    pageChanged = true;
                    if (toggleStorage[i].status) {
                        toggleStorage[i].status = false;
                    } else {
                        toggleStorage[i].status = true;
                    }
                }
            }

        }

};

struct Notification {
    const char* text;
    vex::color displayColor = white;
    double disapearTime;
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
        Notification notifications[10];
        int notifNumber = 0;
        bool showingNotifications = false;

        void shiftNotifications() {
            for (int i=0; i < 10; i++) {
                notifications[i] = notifications[i + 1];
            }
        };

        void drawNotification(int rowNum, Notification notif) {
            int screenXSize = 480;
            //int width = 100;
            int width = Brain.Screen.getStringWidth(notif.text) + 20;
            //int height = 40;
            int height = Brain.Screen.getStringHeight(notif.text) + 10;

            Brain.Screen.drawRectangle(screenXSize - width, (rowNum * height), width, height);
            Brain.Screen.setPenColor(notif.displayColor);
            Brain.Screen.printAt(screenXSize - (width / 2) - (Brain.Screen.getStringWidth(notif.text) / 2), (rowNum * height) + (height / 2) + (Brain.Screen.getStringHeight(notif.text) / 4), notif.text);
            Brain.Screen.setPenColor(white);
        }

    public:

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
                for (int i = 0; i < notifNumber; i++) {
                    drawNotification(i, notifications[i]);
                    if (notifications[i].disapearTime < Brain.timer(msec)) {
                        shiftNotifications();
                        notifNumber--;
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
            pageStorage[displayPage]->screenPressed(Brain.Screen.xPosition(), Brain.Screen.yPosition());
        };



        void newNotification(const char* text, int displayTime, vex::color displayColor = white) {
            notifications[notifNumber] = Notification();
            notifications[notifNumber].text = text;
            notifications[notifNumber].displayColor = displayColor;
            notifications[notifNumber].disapearTime = Brain.timer(msec) + (1000.00 * displayTime);
            notifNumber++;
        };

};



struct NotCheck {
    const char* trueMessage;
    vex::color trueColor = green;
    const char* falseMessage;
    vex::color falseColor = red;
    bool log;
    bool lastVal;
    bool (*checkCB)();
    
};

class NotificationChecker {
    private:
        NotCheck checksStorage[10];
        int checksStored = 0;

        MenuSystem* menuSystemPtr;

    public:
        NotificationChecker(MenuSystem* ptr) {
            menuSystemPtr = ptr;
        };

        void addCheck(const char* trueMessage, const char* falseMessage, bool (*cb)(), bool initVal = false, vex::color trueColor = green, vex::color falseColor = red, bool log = false) {
            NotCheck newCheck = NotCheck();
            newCheck.trueMessage = trueMessage;
            newCheck.falseMessage = falseMessage;
            newCheck.checkCB = cb;
            newCheck.falseColor = falseColor;
            newCheck.trueColor = trueColor;
            newCheck.lastVal = initVal;
            newCheck.log = log;
            checksStorage[checksStored] = newCheck;
            checksStored++;
        }
        void check() {
            for (int i = 0; i < checksStored; i++) {
                bool newVal = checksStorage[i].checkCB();
                if (newVal != checksStorage[i].lastVal) {
                    if (checksStorage[i].log) {
                        brainFancyDebug(newVal ? checksStorage[i].trueMessage : checksStorage[i].falseMessage, newVal ? checksStorage[i].trueColor : checksStorage[i].falseColor, true);
                    } else {
                        menuSystemPtr->newNotification(newVal ? checksStorage[i].trueMessage : checksStorage[i].falseMessage, 4, newVal ? checksStorage[i].trueColor : checksStorage[i].falseColor);
                    } 
                }
                checksStorage[i].lastVal = newVal;
            }
        }
};