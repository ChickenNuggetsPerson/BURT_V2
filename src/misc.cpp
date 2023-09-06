
#include "robotConfig.h"
#include "misc.h"

#include <iostream>
#include <fstream>
#include <sys/stat.h>

vex::mutex sdReaderMutex;

int misc::readFile(const char* fileName) {
  //DEBUGLOG("Reading File: ", fileName);
  sdReaderMutex.lock();
  std::fstream readStream(fileName, std::ios_base::in);
  int readVal;
  readStream >> readVal;
  sdReaderMutex.unlock();
  return readVal;
}
double misc::readFileDouble(const char* fileName) {
  //DEBUGLOG("Reading File: ", fileName);
  sdReaderMutex.lock();
  std::fstream readStream(fileName, std::ios_base::in);
  int readVal;
  readStream >> readVal;
  sdReaderMutex.unlock();
  return readVal;
}

void misc::writeFile(const char* fileName, int numToWrite) {
  //DEBUGLOG("Writing File: ", fileName);
  sdReaderMutex.lock();
  std::ofstream writeStream(fileName);

  std::ostringstream writeString;
  writeString << numToWrite;

  writeStream << writeString.str();
  writeStream.close();
  sdReaderMutex.unlock();
}
void misc::writeFile(const char* fileName, double numToWrite) {
  //DEBUGLOG("Writing File: ", fileName);
  sdReaderMutex.lock();
  std::ofstream writeStream(fileName);

  std::ostringstream writeString;
  writeString << numToWrite;

  writeStream << writeString.str();
  writeStream.close();
  sdReaderMutex.unlock();
};

void misc::writeFile(const char* fileName, const char* content) {
  //DEBUGLOG("Writing File: ", fileName);
  sdReaderMutex.lock();
  std::ofstream writeStream(fileName);
  writeStream << content;
  writeStream.close();
  sdReaderMutex.unlock();
};

void misc::appendFile(const char* fileName, int numToAppend) {
  //DEBUGLOG("Appending File: ", fileName);
  sdReaderMutex.lock();
  std::ofstream writeStream(fileName, std::ios_base::app);

  std::ostringstream writeString;
  writeString << "\n" << numToAppend;

  writeStream << writeString.str();
  writeStream.close();
  sdReaderMutex.unlock();
}

void misc::appendFile(const char* fileName, const char* content) {
  //DEBUGLOG("Appending File: ", fileName);
  sdReaderMutex.lock();
  std::ofstream writeStream;
  writeStream.open(fileName, std::ios_base::app);
  writeStream << "\n" << content;
  writeStream.close();
  sdReaderMutex.unlock();
};

bool misc::copyFile(const char* file, const char* dest) {
  //DEBUGLOG("Copying File: ", file);
  sdReaderMutex.lock();
  std::string line;
  std::ifstream origonal{file};
  std::ofstream outFile{dest};

  if (origonal && outFile) {
    while (std::getline(origonal, line)) {
      outFile << line << "\n";
    }
    sdReaderMutex.unlock();
    return true;
  } else {
    origonal.close();
    outFile.close();
    sdReaderMutex.unlock();
    return false;
  }
}


bool misc::fileExists(const char* name) {
  //DEBUGLOG("Checking File: ", name);
  sdReaderMutex.lock();
  if (FILE *file = fopen(name, "r")) {
      fclose(file);
      sdReaderMutex.unlock();
      return true;
  } else {
      sdReaderMutex.unlock();
      return false;
  }   
}



double misc::degreeToRad(double degree) {
  return degree * ( PI / 180 );
};
double misc::radToDegree(double rad) {
  return rad * ( 180 / PI );
};


double misc::limitAngle(double angle) {
  return fmod(angle, 360);
};



// Example Code I found Online... I Will have to do more research on this
DynamicJsonDocument* misc::readJsonFromFile(const std::string& filePath) {
  sdReaderMutex.lock();
  std::ifstream file(filePath);  // Open the file and position the stream at the end
  if (!file.is_open()) { 
    sdReaderMutex.unlock();
    return new DynamicJsonDocument(1); 
  }

  const int fileSize = 1024 * 4;
  std::vector<char> buffer(fileSize);
  file.read(buffer.data(), fileSize);  // Read the file content into the buffer

  DynamicJsonDocument* jsonBuffer = new DynamicJsonDocument(1024 * 5);
  jsonBuffer->clear();

  DeserializationError error = deserializeJson(*jsonBuffer, buffer.data());

  file.close();
  sdReaderMutex.unlock();

  // Fix this... For some reason, it always says No Memory
  switch (error.code()) {
    case DeserializationError::Ok:
        sdReaderMutex.unlock();
        return jsonBuffer;
        break;
    case DeserializationError::InvalidInput:
        brainError(error.c_str());
        break;
    case DeserializationError::NoMemory:
        brainError(error.c_str());
        break;
    default:
        brainError(error.c_str());
        break;
  }

    
  sdReaderMutex.unlock();
  return new DynamicJsonDocument(1);
}

bool misc::writeJsonToFile(const std::string& filePath, const DynamicJsonDocument& jsonData) {
  sdReaderMutex.lock();
  std::ofstream file(filePath);
  if (file.is_open()) {
    serializeJson(jsonData, file);
    file.close();
    sdReaderMutex.unlock();
    return true;
  } else {
    sdReaderMutex.unlock();
    return false;
  }
}