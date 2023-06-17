
#include "robotConfig.h"
#include "misc.h"

#include <iostream>
#include <fstream>
#include <sys/stat.h>


int readFile(const char* fileName) {
  std::fstream readStream(fileName, std::ios_base::in);
  int readVal;
  readStream >> readVal;
  return readVal;
}

void writeFile(const char* fileName, int numToWrite) {
  std::ofstream writeStream(fileName);

  std::ostringstream writeString;
  writeString << numToWrite;

  writeStream << writeString.str();
  writeStream.close();
}

void writeFile(const char* fileName, const char* content) {
  std::ofstream writeStream(fileName);
  writeStream << content;
  writeStream.close();
};

void appendFile(const char* fileName, int numToAppend) {
  std::ofstream writeStream(fileName, std::ios_base::app);

  std::ostringstream writeString;
  writeString << "\n" << numToAppend;

  writeStream << writeString.str();
  writeStream.close();
}

void appendFile(const char* fileName, const char* content) {
  std::ofstream writeStream;
  writeStream.open(fileName, std::ios_base::app);
  writeStream << "\n" << content;
  writeStream.close();
};

bool copyFile(const char* file, const char* dest) {
  std::string line;
  std::ifstream origonal{file};
  std::ofstream outFile{dest};

  if (origonal && outFile) {
    while (std::getline(origonal, line)) {
      outFile << line << "\n";
    }
    return true;
  } else {
    origonal.close();
    outFile.close();
    return false;
  }
}


bool fileExists(const char* name) {
    if (FILE *file = fopen(name, "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
}


std::map<std::string, std::string> getAllDirs() {
  std::map<std::string, std::string> dirs;

  std::cout << std::endl;
  Directory mainDirectory = Directory(std::string("main"));
  mainDirectory.addFile("test.txt");
  mainDirectory.addFile("other/test.txt");
  mainDirectory.addFile("other/yeet.txt");
  mainDirectory.addFile("other/yeet/yeet.txt");
  mainDirectory.addFile("other/yeet/fun/stuff/among/us.txt");
  mainDirectory.addFile("main/fun.txt");

  std::vector<std::string> result = mainDirectory.listEverything();
  for (int i = 0; i < result.size(); i++) {
    std::cout << result.at(i).c_str() << std::endl;
  }

  return dirs;
};




double degreeToRad(double degree) {
  return degree * ( PI / 180 );
};
double radToDegree(double rad) {
  return rad * ( 180 / PI );
};


double limitAngle(double angle) {
  return fmod(angle, 360);
};