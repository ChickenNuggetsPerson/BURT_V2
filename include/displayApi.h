#pragma once

#include "vex.h"
#include "string"
#include "robotConfig.h"
#include "vector"

using namespace vex;

namespace display {

    // Helper Function
    bool inRectangle(int posX, int posY, int x, int y, int width, int height);

    // A base log message class
    // It contains the actual text of the log and the display color
    struct LogMessage {
        std::string text;
        vex::color displayColor;
        int displayNumber;
        double time;
        
        LogMessage(std::string message = "", vex::color messageColor = vex::color::white, int displayData = 0 ) {
            text = message;
            displayColor = messageColor;
            displayNumber = displayData;
            time = Brain.timer(timeUnits::sec);

            if (time < 10) {
                time = floor(time * 100) / 100;
            }
            if (time >= 10 && time < 100) {
                time = floor(time * 10) / 10;
            } 
            if (time >= 100) {
                time = floor(time);
            } 

        };
    };

    // The main class for controlling logging for the robot
    // Handles displaying the logs on the screen and writing them to file
    class Logger {
        private:

            bool initialized = false;

            int xPos;
            int yPos;
            vex::fontType displayFont = fontType::mono20;

            int maxRows;
            int currentLine;

            const char* logFile;
            bool isSaving = false;
            bool compLogs = false;
            const char* compLogPath;

            std::vector <LogMessage> brainLog;

            void brainLogShift();
            void appendLogFile(const char* message);

        public:
            Logger(int renderX, int renderY);
            Logger(int renderX, int renderY, const char* outFile, int maxLogs, vex::fontType font);

            void init() { isSaving = Brain.SDcard.isInserted(); misc::writeFile(logFile, "Start of Logs"); }

            void reloadLogger(const char* outFile);
            void newLog(const char* message, vex::color messageColor);
            void newLog(const char* message, vex::color messageColor, int data);
            
            void render();


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
            bool inRange(int value);
            bool isDefault();
    };

    class rgbColor {
        private:
        public:
            int r = NAN;
            int g = NAN;
            int b = NAN;

            rgbColor() {};
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
    
            bool isDefault() {
                return isnan(r) || isnan(g) || isnan(b);
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
            void buildHSV(double startHue, double endHue);
            Gradient(rgbColor startColor, rgbColor endColor, int startOfRange, int endOfRange) {
                rangeStart = startOfRange;
                rangeEnd = endOfRange;
                buildRGB(startColor, endColor);
            };
            void buildRGB(rgbColor startColor, rgbColor endColor);
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

            void shiftDataPoints();
            vex::color determinColorFromRange(int value, std::vector<colorRange> ranges);


        public:

            const char* getId();
            void build(const char* graphId, const char* graphName, int xPos, int yPos, int graphWidth, int graphHeight, bool drawMiddle, std::vector<colorRange> colorRanges);
            void build(const char* graphId, const char* graphName, int xPos, int yPos, int graphWidth, int graphHeight, bool drawMiddle, std::vector<colorRange> colorRanges, int maxDataPoints);

            void addPoint(int point);

            void draw();
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
        bool fillGradient = true;
        std::vector<colorRange> ranges;
        
        ProgressBar(const char* barId, int x, int y, int width, int height, const char* name, bool vertical, bool middle, bool fillGradient, std::vector<colorRange> ranges) {
            this->id = barId;
            this->x = x;
            this->y = y;
            this->width = width;
            this->height = height;
            this->name = name;
            this->middle = middle;
            this->fillGradient = fillGradient;
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

    // Forward decloration
    class Page; 
    class MenuSystem;
    class AdjustableNum;

    struct Button {
        int x;
        int y;
        int width;
        int height;
        const char* id;
        std::string text;
        int data;
        vex::color fillColor;
        int (*cb)(Page*);
        Button(const char* text, int x, int y, int width, int height, int (*cb)(Page*), const char* id = "", vex::color fillColor = black) {
            this->text = std::string(text);
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

            void render();
            bool checkPressed(int posX, int posY);

            const char* getId();

            void increase();
            void decrease();

            double getVal();
            void setVal(double newVal);
            std::string getDisplayVal();

    };

    class Plot {

        private:

            MenuSystem* systemPtr;
            odom::Position posAtDist(odom::Position currentPosition, int dist);

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


            odom::Position point1;
            bool drawPoint1 = false;
            odom::Position point2;
            bool drawPoint2 = false;

            bool drawStoredLine = false;

            bool drawFeild = false;
            int mainColor = TEAM_NULL;

            bool drawingPath = false;
            auton::autonPath path;

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

            void drawGameElements();

            void draw();
            void drawLine(odom::Position pos1, odom::Position pos2);
            void drawLine(odom::Position pos1, odom::Position pos2, int penWidth);
            void updatePoint(int pointNum, bool draw);
            void updatePoint(int pointNum, bool draw, odom::Position newPoint);

            void updateLine (bool draw);
            void drawPoint(odom::Position drawPoint, vex::color displayColor);
            void drawPoints();

            void showPath(auton::autonPath displayPath);
    };

    class Page {
        private:

            Logger* loggerStorage;
            bool hasLogger = false;

            OverlayQuestion storedOverlay;
            bool showOverlay = false;
            
            rgbColor backgroundColor;
            color backgroundVexColor = color::black;

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


            vex::color determinColorFromRange(int value, std::vector<colorRange>& ranges);
            

            void drawHorzProgressbar(ProgressBar& bar);
            void drawVertProgressBar(ProgressBar& bar);
            void drawButton(Button& button);
            void drawDisplayBox(DisplayBox& box);
            void drawToggle(Toggle& toggleButton);

            int screenXSize = 480;
            int screenYSize = 240;

            int overlayWidth = 300;
            int overlayHeight = 150;

            void drawOverlay(OverlayQuestion& overlay);


        public:

            std::string identifier;

            bool hasCB = false;
            MenuSystem* menuSystemPointer = nullptr; // So then the page can access the MenuSystem controlling it

            bool pageChanged = false;

            Page() {};

            void render();

            void addDataUpdaterCB(int (*cb)(Page*));
            void addDataUpdaterCB(int (*cb)(Page*), double refreshRate);
            void addPageLoadedCB(int (*cb)(Page*));


            bool updaterRunning = false;
            int updateData();
            void stopUpdater();
            void pageLoaded();


            void addLogger(Logger* loggerMemLocation) {
                loggerStorage = loggerMemLocation;
                hasLogger = true;
            };
            void addHorzProgressBar(const char* barId, int x, int y, int width, int height, const char* name, bool middle, bool fillGradient, std::vector<colorRange> ranges) {
                barStorage.push_back(ProgressBar(barId, x, y, width, height, name, false, middle, fillGradient, ranges));
            };
            void addVertProgressBar(const char* barId, int x, int y, int width, int height, const char* name, bool middle, bool fillGradient, std::vector<colorRange> ranges) {
                barStorage.push_back(ProgressBar(barId, x, y, width, height, name, true, middle, fillGradient, ranges));
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
            void addBackgroundColor(rgbColor backgroundColor) {
                this->backgroundColor = backgroundColor;
                this->backgroundVexColor.rgb(backgroundColor.r, backgroundColor.g, backgroundColor.b);
            }


            void setProgressBarValue(const char* barId, int value);
            void setLineGraphValue(const char* graphId, int value);
            void setTextData(const char* textId, vex::color displayColor);
            void setTextData(const char* textId, vex::color displayColor, const char* newText);
            void setTextData(const char* textId, int data);
            void setTextData(const char* textId, double doubleData);
            void setButtonData(const char* buttonId, vex::color fillColor);
            void setButtonData(const char* buttonId, vex::color fillColor, int data);
            void setButtonData(const char* buttonId, vex::color fillColor, const char* newText);
            void setDisplayBoxData(const char* boxId, vex::color fillColor);
            void setDisplayBoxData(const char* boxId, vex::color fillColor, vex::color penColor);
            void setToggleStatus(const char* toggleId, bool status);
            Plot* getPlotPointer(const char* id);




            void massSetData(const char* id, int data);


            bool getToggleStatus(const char* toggleId);

            AdjustableNum* getCurrentAdjustNum();
            double getAdjustNum(const char* id);

            // Returns false for option1 and true for option2
            bool overlayQuestion(OverlayQuestion overlay);

            // Returns -1 for option 1, 1 for option 2, 0 for off screen, 10 for the box
            int advOverlayQuestion(OverlayQuestion overlay);

            void screenPressed(int x, int y);

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

    class MenuSystem {
        private:

            std::vector<Page> pageStorage;
            Page* renderPagePtr = nullptr;

            int currentPage = -1;
            int prevPage = 0;

            bool firstTimeRender = true;

            task updaterTask;
            bool isUpdating = false;

            void startUpdaterTask(Page* pagePtr);
            void stopUpdaterTask();


            task notificationTask;
            std::vector<Notification> notifications;
            bool showingNotifications = false;

            void drawNotification(int rowNum, Notification notif);

        public:

            int teamColor = 0;

            MenuSystem(bool displayNotifications) {
                showingNotifications = displayNotifications;
            };
        
            void ready();

            void render();
            void addPage(const char* pageId, Page& page);
            void gotoPage(const char* pageId);
            void gotoPage(int index);
            void gotoPrevPage();
            Page* searchPages(const char* pageId);
            void screenPressed();

            void newNotification(const char* text, int displayTime);
            void newNotification(const char* text, int displayTime, vex::color displayColor);

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

            bool checkMotor(MotCheck check);

        public:
            NotificationChecker(MenuSystem* ptr) {
                menuSystemPtr = ptr;
            };

            void addCheck(const char* trueMessage, const char* falseMessage, bool (*cb)());
            void addCheck(const char* trueMessage, const char* falseMessage, bool (*cb)(), bool initVal);
            void addCheck(const char* trueMessage, const char* falseMessage, bool (*cb)(), bool initVal, vex::color trueColor);
            void addCheck(const char* trueMessage, const char* falseMessage, bool (*cb)(), bool initVal, vex::color trueColor, vex::color falseColor);
            void addCheck(const char* trueMessage, const char* falseMessage, bool (*cb)(), bool initVal, vex::color trueColor, vex::color falseColor, bool log);
            void addMotor(const char* motorName, motor* motorPtr);
            void addMotor(const char* motorName, motor* motorPtr, int warnTime);
            void check();
    };
}
