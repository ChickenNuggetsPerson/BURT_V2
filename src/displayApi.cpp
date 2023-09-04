#include "displayApi.h"

using namespace display;


bool display::inRectangle(int posX, int posY, int x, int y, int width, int height) {
    return (posX > x && posX < x + width) && (posY > y && posY < y + height);
};

// Task Functions

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
int mainDataUpdater(void* pageToUpdate) {
    Page* pagePointer = (Page*)pageToUpdate;
    pagePointer->updateData();
    return 1;
}
bool notMotorCheck(void* motorToCheck) {
    motor* motorPointer = (motor*)motorToCheck;
    return motorPointer->temperature(temperatureUnits::fahrenheit) >= motorWarnTemp;
}



// Logger Functions
Logger::Logger(int renderX, int renderY) {
    xPos = renderX;
    yPos = renderY;
    maxRows = 11;

    logFile = "logs.txt";

};
Logger::Logger(int renderX, int renderY, const char* outFile = "logs.txt", int maxLogs = 11, vex::fontType font = fontType::mono20) {
    xPos = renderX;
    yPos = renderY;
    maxRows = maxLogs;
    displayFont = font;

    logFile = outFile;
    

    //reloadLogger(logFile);
    
};
void Logger::brainLogShift() {
    brainLog.erase(brainLog.begin());
};
void Logger::appendLogFile(const char* message) {
    if (!isSaving) { return; }

    std::ostringstream formattedMessage;
    formattedMessage << "[ " << Brain.timer(seconds) << " ] " << message;

    misc::appendFile(logFile, formattedMessage.str().c_str());
    if (compLogs) {
        misc::appendFile(compLogPath, formattedMessage.str().c_str()); // Todo: Fix this 
    }
};
void Logger::reloadLogger(const char* outFile) {
    const char* empty;
    if (strcmp(outFile, empty) == 0) {
        logFile = "logs.txt";
    } else {
        logFile = outFile;
    }
    isSaving = Brain.SDcard.isInserted();
    if (!isSaving) { 
        initialized = true;   
        return; 
    }

    compLogs = Competition.isFieldControl();

    std::string path;
    path += systemConfigFolder;
    path += systemArchivePath;
    if (misc::readFile(path.c_str()) == 1) {
        DEBUGLOG(" ", " ");
        DEBUGLOG(" ", " ");
        DEBUGLOG(" ", " ");

        if (misc::fileExists(logFile)) {
            // Save the last logs to the archive folder
            int logNum = 0;
            bool looking = true;
            DEBUGLOG("Starting Log Looking")
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
                looking = misc::fileExists(tmpName.c_str());
            }
            std::string cpyDest;
            cpyDest += systemLogFolder;
            cpyDest += systemArchiveFolder;

            std::ostringstream str;
            str << logNum;
            cpyDest += str.str();

            cpyDest += ".txt";

            DEBUGLOG("Archiving to: ", cpyDest);

            misc::copyFile(logFile, cpyDest.c_str());
        }
    } else {
        DEBUGLOG(" ");
        DEBUGLOG(" ");
        DEBUGLOG("Not Archiving");
    }
    if (compLogs) {
        int logNum = 0;
        bool looking = true;
        DEBUGLOG("Starting Comp Log Looking");
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
            looking = misc::fileExists(tmpName.c_str());
        }
        std::string cpyDest;
        cpyDest += systemLogFolder;
        cpyDest += systemCompLogsFolder;
        std::ostringstream str;
        str << logNum;
        cpyDest += str.str();
        cpyDest += ".txt";

        compLogPath = cpyDest.c_str();

        DEBUGLOG("Archiving To: ", compLogPath);

        misc::writeFile(compLogPath, "Start of Comp Logs");

    }

    misc::writeFile(logFile, "Start of Logs");
    initialized = true;  
}
void Logger::newLog(const char* message, vex::color messageColor) {
    newLog(message, messageColor, 0);
}
void Logger::newLog(const char* message, vex::color messageColor, int data) {

    if (currentLine == maxRows) {
        brainLogShift();
        currentLine = maxRows - 1;
    }

    brainLog.push_back(LogMessage(message, messageColor, data));
    currentLine ++;

    appendLogFile(message);
};
void Logger::render() {
    // Display Log Messages
    Brain.Screen.setFont(displayFont);
    for (int i = 0; i < brainLog.size(); i++) {
        Brain.Screen.setCursor(i + yPos, xPos);
        std::ostringstream formattedMessage;
        formattedMessage << "[ " << brainLog.at(i).time << " ] ";
        Brain.Screen.setPenColor(white);
        Brain.Screen.print(formattedMessage.str().c_str());     

        Brain.Screen.setCursor(i + yPos, xPos + 11);
        Brain.Screen.setPenColor(brainLog.at(i).displayColor);
        Brain.Screen.print(brainLog.at(i).text.c_str(), brainLog.at(i).displayNumber);     
        Brain.Screen.setPenColor(vex::color::white);
    }
};


// Color Range Functions
bool colorRange::inRange(int value) {
    return (value >= start && value <= end);
}
bool colorRange::isDefault() {
    return start == NAN && end == NAN;
}


// Gradient Functions
void Gradient::buildHSV(double startHue, double endHue) {

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
void Gradient::buildRGB(rgbColor startColor, rgbColor endColor) {

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


// Graph Functions
void Graph::shiftDataPoints() {
    for (int i=0; i < maxPoints - 1; i++) {
        dataPoints[i] = dataPoints[i + 1];
    }
};
vex::color Graph::determinColorFromRange(int value, std::vector<colorRange> ranges) {
    for (auto range: ranges) {
        if (range.inRange(value)) {
            return range.displayColor;
        }  
    }
    return vex::color::black;
}
const char* Graph::getId() {
    return graphId;
};
void Graph::build(const char* graphId, const char* graphName, int xPos, int yPos, int graphWidth, int graphHeight, bool drawMiddle, std::vector<colorRange> colorRanges) {
    build(graphId, graphName, xPos, yPos, graphWidth, graphHeight, drawMiddle, colorRanges, 10);
};
void Graph::build(const char* graphId, const char* graphName, int xPos, int yPos, int graphWidth, int graphHeight, bool drawMiddle, std::vector<colorRange> colorRanges, int maxDataPoints) {
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
void Graph::addPoint(int point) {
    if (currentDataPoint == maxPoints) {
        shiftDataPoints();
        currentDataPoint = maxPoints - 1;
    }
    dataPoints[currentDataPoint] = point;
    currentDataPoint ++;

    recentPoint = point;
};
void Graph::draw() {
            
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


// Adjustable Number Functions
void AdjustableNum::render() {

    std::stringstream displayValStream;
    displayValStream << val;
    const char* displayVal = displayValStream.str().c_str();

    Brain.Screen.drawRectangle(x, y, width, height);
    Brain.Screen.setFont(font);
    Brain.Screen.printAt(x + (width / 2) - (Brain.Screen.getStringWidth(displayVal) / 2), y + (height / 2) + (Brain.Screen.getStringHeight(displayVal) / 4), displayVal);

}
bool AdjustableNum::checkPressed(int posX, int posY) {
    return inRectangle(posX, posY, x, y, width, height);
}
const char* AdjustableNum::getId() {
    return title;
}
void AdjustableNum::increase() {
    val += stepSize;
    if (val > maxVal) { val = maxVal; }
};
void AdjustableNum::decrease() {
    val -= stepSize;
    if (val < minVal) { val = minVal; }
}
double AdjustableNum::getVal() {
    return val;
}
void AdjustableNum::setVal(double newVal) {
    double tmpVal = newVal;
    if (tmpVal > maxVal) { tmpVal = maxVal; }
    if (tmpVal < minVal) { tmpVal = minVal; }
    val = tmpVal;
}
std::string AdjustableNum::getDisplayVal() {
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


// Plot Functions
odom::Position Plot::posAtDist(odom::Position currentPosition, int dist) {
    double desiredX = currentPosition.x + dist * cos(2.5*PI - currentPosition.rot);
    double desiredY = currentPosition.y + dist * sin(2.5*PI - currentPosition.rot);
    return odom::Position(desiredX, desiredY);
}
void Plot::drawGameElements() {

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
void Plot::draw() {
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
        int lineWidth = 4;

        odom::Position currentPos = path.startPos;
        int i = 0;
        while (true) {
            auton::autonMovement move = path.getStep(i);
            if (move.movementType == AUTON_MOVE_END) { break; }
            odom::Position newPos;
            switch (move.movementType) {
                case AUTON_MOVE_DELAY:
                    
                    break;
                case AUTON_MOVE_DRIVE_DIST:
                    newPos = posAtDist(currentPos, move.val);
                    drawLine(currentPos, newPos, lineWidth);
                    currentPos = newPos;
                    break;
                case AUTON_MOVE_GOTO:
                    if (move.tilePosBool) {
                        newPos = odom::tilePosToPos(move.tilePos);
                        drawLine(currentPos, newPos, lineWidth);
                        currentPos = newPos;
                    } else {
                        drawLine(currentPos, move.pos, lineWidth);
                        currentPos = move.pos;                                
                    }
                    break;
                case AUTON_MOVE_LONGGOTO:
                    for (int i = 0; i < move.drivePath.size(); i++) {
                        newPos = odom::tilePosToPos(move.drivePath.at(i));
                        drawLine(currentPos, newPos, lineWidth);
                        currentPos = newPos;
                    }
                    break;
                case AUTON_MOVE_TURNTO:
                    //return pointer->turnTo(move.pos.rot);
                    break;
                case AUTON_MOVE_PICKUP_ACORN:
                    //return pointer->pickupAcorn();
                    break;
                case AUTON_MOVE_DROPOFF_ACORN:
                    //return pointer->dropAcorn();
                    break;
            }
            i++;
            

        }
            
    } else {
        drawPoints();
    }
    
};
void Plot::drawLine(odom::Position pos1, odom::Position pos2) {
    drawLine(pos1, pos1, 1);
}
void Plot::drawLine(odom::Position pos1, odom::Position pos2, int penWidth) {
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
void Plot::updatePoint(int pointNum, bool draw) {
    updatePoint(pointNum, draw, odom::Position());
} 
void Plot::updatePoint(int pointNum, bool draw, odom::Position newPoint) {
    if (pointNum == 1) {
        point1 = newPoint;
        drawPoint1 = draw;
    }
    if (pointNum == 2) {
        point2 = newPoint;
        drawPoint2 = draw;
    }
}
void Plot::updateLine (bool draw) {
    drawStoredLine = draw;
}
void Plot::drawPoint(odom::Position drawPoint, vex::color displayColor) {
    double radius = 5;

    int drawX = (( drawPoint.x / maxX ) * width) + x;
    int drawY = y + 5 + height - (( drawPoint.y / maxY) * height);

    Brain.Screen.setFillColor(displayColor);
    Brain.Screen.drawCircle(drawX, drawY, radius);
    Brain.Screen.setFillColor(black);
}
void Plot::drawPoints() {

    if (drawPoint2) {
        drawPoint(point2, cyan);
    }
    if (drawPoint1) {
        drawPoint(point1, color::green);
    }
};
void Plot::showPath(auton::autonPath displayPath) {
    path = displayPath;
    drawingPath = true;
}


// Page Function
vex::color Page::determinColorFromRange(int value, std::vector<colorRange>& ranges) {
    for (auto range: ranges) {
        if (range.inRange(value)) {
            return range.displayColor;
        }  
    }
    return vex::color::black;
}
void Page::drawHorzProgressbar(ProgressBar& bar) {

    if (bar.fillGradient) {

        // Normal Bar Fill
        // Fills whole bar with the current color
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
    } else {

        // Actually applies the gradient to the bar
        int roundedVal = trunc(bar.value);
        double segmentWidth = bar.width / 100.00;

        if (bar.middle) {
            // Draw Middle
            segmentWidth = segmentWidth / 2.00;
            double actualWidth = segmentWidth;
            if (segmentWidth < 2) {
                segmentWidth = 2;
            }
            if (roundedVal >= 0) {
                    // Going up
                for (int i = 0; i <= roundedVal; i++) {
                    color displayColor = determinColorFromRange(i, bar.ranges);
                    Brain.Screen.setFillColor(displayColor);
                    Brain.Screen.setPenColor(displayColor);
                    Brain.Screen.drawRectangle(bar.x + (i * actualWidth) + (bar.width / 2.00), bar.y + 5, segmentWidth, bar.height);
                }
            } else {
                    // Going Down
                for (int i = roundedVal; i <= 0; i++) {
                    color displayColor = determinColorFromRange(i, bar.ranges);
                    Brain.Screen.setFillColor(displayColor);
                    Brain.Screen.setPenColor(displayColor);
                    Brain.Screen.drawRectangle(bar.x + (i * actualWidth) + (bar.width / 2.00), bar.y + 5, segmentWidth, bar.height);
                    
                }
            }
        } else {
            // Draw Normal
            if (roundedVal >= 0) {
                 // Going up
                for (int i = 0; i <= roundedVal; i++) {
                    Brain.Screen.setFillColor(determinColorFromRange(i, bar.ranges));
                    Brain.Screen.setPenColor(color::transparent);
                    Brain.Screen.setPenWidth(0);
                    if (i == roundedVal) {
                        Brain.Screen.drawRectangle(bar.x + (i * segmentWidth), bar.y + 5, segmentWidth, bar.height);
                    } else {
                        Brain.Screen.drawRectangle(bar.x + (i * segmentWidth), bar.y + 5, segmentWidth * 2, bar.height);
                    }
                }
            } else {
                 // Going Down
                for (int i = roundedVal; i <= 0; i++) {
                    Brain.Screen.setFillColor(determinColorFromRange(i, bar.ranges));
                    Brain.Screen.setPenColor(color::transparent);
                    Brain.Screen.setPenWidth(0);
                    if (i == 0) {
                        Brain.Screen.drawRectangle(bar.x + (i * segmentWidth), bar.y + 5, segmentWidth, bar.height);
                    } else {
                        Brain.Screen.drawRectangle(bar.x + (i * segmentWidth), bar.y + 5, segmentWidth * 2, bar.height);
                    }
                }
            }
        }
        Brain.Screen.setFillColor(vex::color::black);
    }

    Brain.Screen.setFillColor(backgroundVexColor);
    Brain.Screen.setPenColor(color::white);
    Brain.Screen.setPenWidth(1);
    Brain.Screen.printAt(bar.x, bar.y, bar.name, int(bar.value));
    Brain.Screen.setFillColor(color::transparent);
    Brain.Screen.drawRectangle(bar.x, bar.y + 5, bar.width, bar.height);  
};
void Page::drawVertProgressBar(ProgressBar& bar) {
    
    if (bar.fillGradient) {
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
    } else {
        // Actually applies the gradient to the bar
        int roundedVal = trunc(bar.value);
        double segmentHeight = bar.height / 100.00;

        if (bar.middle) {
            // Draw Middle
            segmentHeight = segmentHeight / 2.00;
            double actualHeight = segmentHeight;
            if (segmentHeight < 2) {
                segmentHeight = 2;
            }
            if (roundedVal >= 0) {
                    // Going up
                for (int i = 0; i <= roundedVal; i++) {
                    color displayColor = determinColorFromRange(i, bar.ranges);
                    Brain.Screen.setFillColor(displayColor);
                    Brain.Screen.setPenColor(displayColor);
                    Brain.Screen.drawRectangle(bar.x, bar.y + 5 + (bar.height / 2) - (i * actualHeight), bar.width, segmentHeight);
                }
            } else {
                    // Going Down
                for (int i = roundedVal; i <= 0; i++) {
                    color displayColor = determinColorFromRange(i, bar.ranges);
                    Brain.Screen.setFillColor(displayColor);
                    Brain.Screen.setPenColor(displayColor);
                    Brain.Screen.drawRectangle(bar.x, bar.y + 5 + (bar.height / 2) - (i * actualHeight), bar.width, segmentHeight);
                    
                }
            }
        } else {
            // Draw Normal
            if (roundedVal >= 0) {
                 // Going up
                for (int i = 0; i <= roundedVal; i++) {
                    color displayColor = determinColorFromRange(i, bar.ranges);
                    Brain.Screen.setFillColor(displayColor);
                    Brain.Screen.setPenColor(displayColor);
                    Brain.Screen.drawRectangle(bar.x, bar.y + 5 + bar.height - (i * segmentHeight), bar.width, segmentHeight);
                    
                }
            } else {
                 // Going Down
                for (int i = roundedVal; i <= 0; i++) {
                    color displayColor = determinColorFromRange(i, bar.ranges);
                    Brain.Screen.setFillColor(displayColor);
                    Brain.Screen.setPenColor(displayColor);
                    Brain.Screen.drawRectangle(bar.x, bar.y + 5 + bar.height - (i * segmentHeight), bar.width, segmentHeight);
                    
                }
            }
        }
        Brain.Screen.setFillColor(vex::color::black);
    }
    
    Brain.Screen.setFillColor(backgroundVexColor);
    Brain.Screen.setPenColor(color::white);
    Brain.Screen.setPenWidth(1);
    Brain.Screen.printAt(bar.x, bar.y, bar.name, int(bar.value));
    Brain.Screen.setFillColor(color::transparent);
    Brain.Screen.drawRectangle(bar.x, bar.y + 5, bar.width, bar.height);
};
void Page::drawButton(Button& button) {
    Brain.Screen.setFillColor(button.fillColor);
    Brain.Screen.drawRectangle(button.x, button.y, button.width, button.height);
    Brain.Screen.setFillColor(backgroundVexColor);
    Brain.Screen.printAt(button.x + (button.width / 2) - (Brain.Screen.getStringWidth(button.text.c_str()) / 2), button.y + (button.height / 2) + (Brain.Screen.getStringHeight(button.text.c_str()) / 3), button.text.c_str(), button.data);
};
void Page::drawDisplayBox(DisplayBox& box) {
    Brain.Screen.setPenColor(box.penColor);
    Brain.Screen.setFillColor(box.fillColor);
    Brain.Screen.drawRectangle(box.x, box.y, box.width, box.height);
    Brain.Screen.setPenColor(color::white);
    Brain.Screen.setFillColor(backgroundVexColor);       
}
void Page::drawToggle(Toggle& toggleButton) {

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
    Brain.Screen.setFillColor(backgroundVexColor);
}
void Page::drawOverlay(OverlayQuestion& overlay) {

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
void Page::render() {

    // Render Background
    if (!backgroundColor.isDefault()) {
        Brain.Screen.setFillColor(backgroundVexColor);
        Brain.Screen.drawRectangle(-5, -5, screenXSize + 10, screenYSize + 10);
    }

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
        Brain.Screen.setFillColor(backgroundVexColor);
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
void Page::addDataUpdaterCB(int (*cb)(Page*)) {
    dataUpdaterCB = cb;
    hasCB = true;
    updateSpeed = 1;
};
void Page::addDataUpdaterCB(int (*cb)(Page*), double refreshRate) {
    dataUpdaterCB = cb;
    hasCB = true;
    updateSpeed = refreshRate;
};
void Page::addPageLoadedCB(int (*cb)(Page*)) {
    pageLoadedCB = cb;
    hasLoadedCB = true;
};
int Page::updateData() {
    updaterRunning = true;
    while (updaterRunning) {
        dataUpdaterCB(this);
        wait(updateSpeed, seconds);                
    }
    return 1;
};
void Page::stopUpdater() {
    updaterRunning = false;
}
void Page::pageLoaded() {
    if (!hasLoadedCB) { return; };
    pageLoadedCB(this);
};
void Page::setProgressBarValue(const char* barId, int value) {
    for (auto &bar: barStorage) {
        if (strcmp(barId, bar.id) == 0) {
            bar.value = value;
            return;
        }
    }
};
void Page::setLineGraphValue(const char* graphId, int value) {
    for (auto &graph: graphStorage) {
        if (strcmp(graphId, graph.getId()) == 0) {
            graph.addPoint(value);
            return;
        }
    }
};
void Page::setTextData(const char* textId, vex::color displayColor) {
    for (auto &text: textStorage) {
        if (strcmp(text.id, textId) == 0) {
            if (displayColor != NAN) {
                text.displayColor = displayColor;
            }
            return;
        }
    }
};
void Page::setTextData(const char* textId, vex::color displayColor, const char* newText) {
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
void Page::setTextData(const char* textId, int data) {
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
void Page::setTextData(const char* textId, double doubleData) {
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
void Page::setButtonData(const char* buttonId, vex::color fillColor) {
    setButtonData(buttonId, fillColor, 0);
}
void Page::setButtonData(const char* buttonId, vex::color fillColor, int data) {
    for (auto &button: buttonStorage) {
        if (strcmp(button.id, buttonId) == 0) {
            button.data = data;
            button.fillColor = fillColor;
            return;
        }
    }
}
void Page::setButtonData(const char* buttonId, vex::color fillColor, const char* newText) {
    for (auto &button: buttonStorage) {
        if (strcmp(button.id, buttonId) == 0) {
            button.text = std::string(newText);
            button.fillColor = fillColor;
            return;
        }
    }
};
void Page::setDisplayBoxData(const char* boxId, vex::color fillColor) {
    setDisplayBoxData(boxId, fillColor, color::white);
}
void Page::setDisplayBoxData(const char* boxId, vex::color fillColor, vex::color penColor) {
    for (auto &displayBox: displayBoxStorage) {
        if (strcmp(displayBox.id, boxId) == 0) {
            displayBox.fillColor = fillColor;
            displayBox.penColor = penColor;
            return;
        }
    }
};
void Page::setToggleStatus(const char* toggleId, bool status) {
    for (auto &toggle: toggleStorage) {
        if (strcmp(toggle.id, toggleId) == 0) {
            toggle.status = status;
            return;
        }
    }
};
Plot* Page::getPlotPointer(const char* id) {
    for (auto &plot: plotStorage) {
        if (strcmp(plot.id, id) == 0) {
            return &plot;
        }
    }
    return nullptr;
}
void Page::massSetData(const char* id, int data) {
    setProgressBarValue(id, data);
    setLineGraphValue(id, data);
};
bool Page::getToggleStatus(const char* toggleId) {
    for (auto toggle: toggleStorage) {
        if (strcmp(toggle.id, toggleId) == 0) {
            return toggle.status;
        }
    }
    return false;
}
AdjustableNum* Page::getCurrentAdjustNum() {
    if (isnan(activeAdjustNum)) {return nullptr;}
    return &adjustNumStorage[activeAdjustNum];
}
double Page::getAdjustNum(const char* id) {
    for (auto num: adjustNumStorage) {
        if (strcmp(num.getId(), id) == 0) {
            return num.getVal();
        }
    }
    return 0.00;
}
bool Page::overlayQuestion(OverlayQuestion overlay) {
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
int Page::advOverlayQuestion(OverlayQuestion overlay) {
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
void Page::screenPressed(int x, int y) {
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


// MenuSystem Functions
void MenuSystem::startUpdaterTask(Page* pagePtr) {
    if (isUpdating) { stopUpdaterTask(); }
    if (!pagePtr->hasCB) { return; }
    updaterTask = task(mainDataUpdater, (void*)pagePtr, task::taskPrioritylow);
    isUpdating = true;
}; 
void MenuSystem::stopUpdaterTask() {
    updaterTask.stop(updaterTask);
    isUpdating = false;
    //brainDebug("Ended Task");
};
void MenuSystem::drawNotification(int rowNum, Notification notif) {
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
void MenuSystem::render() {
    // Render Page
    if (Brain.timer(timeUnits::sec) < sleepAt) {
        if (renderPagePtr == nullptr) {
            Brain.Screen.printAt(20, 100, "No Pages In Storage");
        } else {
            renderPagePtr->render();
            if (firstTimeRender) { startUpdaterTask(renderPagePtr); firstTimeRender = false;}
        }
    } else {
        Brain.Screen.setFont(fontType::mono30);
        Brain.Screen.printAt(20, 30, "Sleeping...");
        Brain.Screen.setFont(fontType::mono15);
        Brain.Screen.printAt(20, 50, "Press the screen to wake up");
        Brain.Screen.setFont(fontType::mono20);
    }
    
    // Render Notifications
    if (showingNotifications) {
        for (int i = notifications.size() - 1; i > -1; i--) {
            if (notifications[i].disapearTime < Brain.timer(msec)) {
                notifications.erase(notifications.begin() + i);
            } else {
                if (currentPage == 0) { return; } // Don't render notifications on the loading page
                drawNotification(i, notifications.at(i));
            }
            
        }
    }
};
void MenuSystem::ready() {
    gotoPage(0);
};
void MenuSystem::addPage(const char* pageId, Page& page) {
    page.menuSystemPointer = this;
    page.identifier = std::string(pageId);
    pageStorage.push_back(page);
};
void MenuSystem::gotoPage(const char* pageId) {
    refreshSleep();
    std::string id = pageId;
    for (int i = 0; i < pageStorage.size(); i++) {
        if (pageStorage.at(i).identifier != pageId) { continue; }
        if (i == currentPage) { return; }
        prevPage = currentPage;
        currentPage = i;
        renderPagePtr = &pageStorage.at(i);
        startUpdaterTask(renderPagePtr);
        renderPagePtr->pageLoaded();
        return;
    }
};
void MenuSystem::gotoPage(int index) {
    refreshSleep();
    if (index >= pageStorage.size()) { return; }
    if (currentPage == index) { return; }
    
    prevPage = currentPage;
    currentPage = index;
    renderPagePtr = &(pageStorage.at(index));
    startUpdaterTask(renderPagePtr);
    renderPagePtr->pageLoaded();
    return;

};
void MenuSystem::gotoPrevPage() {
    gotoPage(prevPage);
}
Page* MenuSystem::searchPages(const char* pageId) {
    std::string id = pageId;
    for (int i = 0; i < pageStorage.size(); i++) {
        if (pageStorage.at(i).identifier != pageId) { continue; }
        return &pageStorage.at(i);
    }
    return nullptr;
}
void MenuSystem::screenPressed() {
    if (Brain.timer(timeUnits::sec) > sleepAt) { refreshSleep(); return;}
    refreshSleep();
    if (currentPage != 0) {
        pageStorage.at(currentPage).screenPressed(Brain.Screen.xPosition(), Brain.Screen.yPosition());
    } else {
        gotoPage("main"); // Skip the loading screen
    }
};
void MenuSystem::newNotification(const char* text, int displayTime) {
    newNotification(text, displayTime, color::white);
}
void MenuSystem::newNotification(const char* text, int displayTime, vex::color displayColor) {
    if (!showingNotifications) {return;}
    notifications.push_back(Notification(text, Brain.timer(msec) + (1000.00 * displayTime), displayColor));
};


// Notification Checker Functions
bool NotificationChecker::checkMotor(MotCheck check) {
    return check.motorPointer->temperature(percent) >= motorWarnTemp;
}
void NotificationChecker::addCheck(const char* trueMessage, const char* falseMessage, bool (*cb)()) {
    checksStorage.push_back(NotCheck(trueMessage, falseMessage, cb, false, color::green, color::red, false));
};
void NotificationChecker::addCheck(const char* trueMessage, const char* falseMessage, bool (*cb)(), bool initVal) {
    checksStorage.push_back(NotCheck(trueMessage, falseMessage, cb, initVal, color::green, color::red, false));
};
void NotificationChecker::addCheck(const char* trueMessage, const char* falseMessage, bool (*cb)(), bool initVal, vex::color trueColor) {
    checksStorage.push_back(NotCheck(trueMessage, falseMessage, cb, initVal, trueColor, color::red, false));
};
void NotificationChecker::addCheck(const char* trueMessage, const char* falseMessage, bool (*cb)(), bool initVal, vex::color trueColor, vex::color falseColor) {
    checksStorage.push_back(NotCheck(trueMessage, falseMessage, cb, initVal, trueColor, falseColor, false));
};
void NotificationChecker::addCheck(const char* trueMessage, const char* falseMessage, bool (*cb)(), bool initVal, vex::color trueColor, vex::color falseColor, bool log) {
    checksStorage.push_back(NotCheck(trueMessage, falseMessage, cb, initVal, trueColor, falseColor, log));
}
void NotificationChecker::addMotor(const char* motorName, motor* motorPtr) {
    motorCheckStorage.push_back(MotCheck(motorName, motorPtr, 30));
}
void NotificationChecker::addMotor(const char* motorName, motor* motorPtr, int warnTime) {
    motorCheckStorage.push_back(MotCheck(motorName, motorPtr, warnTime));
}
void NotificationChecker::check() {
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
