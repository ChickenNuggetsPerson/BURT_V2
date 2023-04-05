
#include "vex.h"

class MenuSystem;

int controllerDisplay();
int brainDisplayer();

void brainChangePage(const char* pageName);
void brainPageChangeData(const char* pageName, const char* pointId, int data);
void brainPageChangeText(const char* pageName, const char* textId, int data);
void brainPageChangeText(const char* pageName, const char* textId, int data, const char* newText);

void brainError(const char* message);
void brainDebug(const char* message);
void brainFancyDebug(const char* message, vex::color messageColor);
void brainFancyDebug(const char* message, vex::color messageColor, int data);