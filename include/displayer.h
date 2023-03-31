
#include "vex.h"

int controllerDisplay();

int brainDisplayer();

void brainError(const char* message);

void brainDebug(const char* message);

void brainFancyDebug(const char* message, vex::color messageColor);
void brainFancyDebug(const char* message, vex::color messageColor, int data);