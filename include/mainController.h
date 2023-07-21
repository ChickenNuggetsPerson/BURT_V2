#pragma once

int controllerTask();
void mainControllerRender();
void mainControllerMessage(const char* text, int timeout);
bool mainControllerOverlay(const char* question, const char* trueOption, const char* falseOption);
int mainControllerPickOption(std::vector <const char*> options);