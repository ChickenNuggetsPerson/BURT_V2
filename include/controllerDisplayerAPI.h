#pragma once

#include "vex.h"
#include "robotConfig.h"

#include "vector"

struct ControllerMessage {
    const char* text = "";
    double endTime = 0;
};

struct ControllerOverlay {
    const char* question;
    const char* option1;
    const char* option2;
    ControllerOverlay(const char* displayQuestion, const char* message1, const char* message2) {
        option1 = message1;
        option2 = message2;
        question = displayQuestion;
    }
};

bool displayOverlay(ControllerOverlay overlay, controller* con) {
    con->Screen.clearScreen();
    con->Screen.setCursor(1, 0);

    con->Screen.print(overlay.question);
    con->Screen.newLine();
    
    std::string option1 = "^ ";
    std::string option2 = "V ";

    option1 += std::string(overlay.option1);
    option2 += std::string(overlay.option2);

    con->Screen.print(option1.c_str());
    con->Screen.newLine();
    con->Screen.print(option2.c_str());

    wait(0.5, seconds);

    con->rumble("..");
    bool choosing = true;
    while (choosing) {
        if (con->ButtonUp.pressing() || con->ButtonDown.pressing()) {
            con->rumble(".");
            return con->ButtonDown.pressing();
            choosing = false;
        }
        wait(0.05, seconds);
    }
}

int pickOption(std::vector <const char*> options, controller* con) {
    int hover = 0;

    con->Screen.clearScreen();
    wait(0.5, seconds);

    con->rumble(".");

    while (true) {

        int tab = floor(hover / 3);
        int deltaTab = hover - ( tab * 3 );


        con->Screen.clearScreen();
        con->Screen.setCursor(1, 0);
        for (int i = 0; i < 3; i++ ) {
            if ((tab * 3) + i >= options.size()) { break; }
            if (deltaTab == i) {
                std::string printString = "* ";
                printString += options.at((tab * 3) + i);
                con->Screen.print(printString.c_str());
            } else {
                std::string printString = "  ";
                printString += options.at((tab * 3) + i);
                con->Screen.print(printString.c_str());
            }
            con->Screen.newLine();
        }


        if (con->ButtonDown.pressing()) {
            hover++;
        }
        if (con->ButtonUp.pressing()) {
            hover--;
        }
        if (hover < 0) { hover = options.size() - 1; }
        if (hover >= options.size()) { hover = 0; }

        if (con->ButtonA.pressing()) {
            con->rumble(".");
            return hover;
        }

        wait(0.15, seconds);
    }
    

}