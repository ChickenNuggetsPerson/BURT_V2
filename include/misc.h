

int readFile(const char* fileName);

void writeFile(const char* fileName, int numToWrite);
void writeFile(const char* fileName, const char* content);

void appendFile(const char* fileName, int numToAppend);
void appendFile(const char* fileName, const char* content);

bool copyFile(const char* file, const char* dest);
bool fileExists(const char* name);


double degreeToRad(double degree);
double radToDegree(double rad);

double limitAngle(double angle);