
#include "vex.h"

int controllerDisplay();
int brainDisplayer();


void brainChangePage(const char* pageName);
void brainPageChangeData(const char* pageName, const char* pointId, int data);


void brainPageChangeText(const char* pageName, const char* textId, vex::color displayColor);
void brainPageChangeText(const char* pageName, const char* textId, const char* newText);
void brainPageChangeText(const char* pageName, const char* textId, int data);


void brainError(const char* message);
void brainError(const char* message, bool showOnController);
void brainDebug(const char* message);
void brainDebug(const char* message, bool notification);
void brainFancyDebug(const char* message, vex::color messageColor);
void brainFancyDebug(const char* message, vex::color messageColor, int data);
void brainFancyDebug(const char* message, vex::color messageColor, bool showNotification);