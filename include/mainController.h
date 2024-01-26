#pragma once


enum WingStates {
    W_loose, W_pos1, W_pos2, W_pos3, W_close, W_CataAlign, W_LeftAutonPoleTouch, W_LeftAutonSwipeTriball
};

void startCatapult();
void stopCatapult();

int controllerTask();
void mainControllerRender();
void mainControllerMessage(const char* text, int timeout);
bool mainControllerOverlay(const char* question, const char* trueOption, const char* falseOption);
int mainControllerPickOption(std::vector <const char*> options);