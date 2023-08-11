
#include "robotConfig.h"
#include "misc.h"

#include <iostream>
#include <fstream>
#include <sys/stat.h>




int misc::readFile(const char* fileName) {
  std::fstream readStream(fileName, std::ios_base::in);
  int readVal;
  readStream >> readVal;
  return readVal;
}
double misc::readFileDouble(const char* fileName) {
  std::fstream readStream(fileName, std::ios_base::in);
  int readVal;
  readStream >> readVal;
  return readVal;
}

void misc::writeFile(const char* fileName, int numToWrite) {
  std::ofstream writeStream(fileName);

  std::ostringstream writeString;
  writeString << numToWrite;

  writeStream << writeString.str();
  writeStream.close();
}
void misc::writeFile(const char* fileName, double numToWrite) {
  std::ofstream writeStream(fileName);

  std::ostringstream writeString;
  writeString << numToWrite;

  writeStream << writeString.str();
  writeStream.close();
};

void misc::writeFile(const char* fileName, const char* content) {
  std::ofstream writeStream(fileName);
  writeStream << content;
  writeStream.close();
};

void misc::appendFile(const char* fileName, int numToAppend) {
  std::ofstream writeStream(fileName, std::ios_base::app);

  std::ostringstream writeString;
  writeString << "\n" << numToAppend;

  writeStream << writeString.str();
  writeStream.close();
}

void misc::appendFile(const char* fileName, const char* content) {
  std::ofstream writeStream;
  writeStream.open(fileName, std::ios_base::app);
  writeStream << "\n" << content;
  writeStream.close();
};

bool misc::copyFile(const char* file, const char* dest) {
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


bool misc::fileExists(const char* name) {
    if (FILE *file = fopen(name, "r")) {
        fclose(file);
        return true;
    } else {
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
  std::ifstream file(filePath, std::ios::ate);  // Open the file and position the stream at the end
  if (file.is_open()) {
    std::streampos fileSize = file.tellg();  // Get the position of the current character in the input stream
    file.seekg(0);  // Reset the position of the stream to the beginning

    std::vector<char> buffer(fileSize);
    file.read(buffer.data(), fileSize);  // Read the file content into the buffer

    DynamicJsonDocument* jsonBuffer = new DynamicJsonDocument(1024 * 5);
    DeserializationError error = deserializeJson(*jsonBuffer, buffer.data());
    file.close();

    // Fix this... For some reason, it always says No Memory
    switch (error.code()) {
      case DeserializationError::Ok:
          return jsonBuffer;
          break;
      case DeserializationError::InvalidInput:
          brainError(error.c_str());
          return nullptr;
          break;
      case DeserializationError::NoMemory:
          brainError(error.c_str());
          return nullptr;
          break;
      default:
          brainError(error.c_str());
          return nullptr;
          break;
    }
    
  } else {
  }
  return nullptr;
}

bool misc::writeJsonToFile(const std::string& filePath, const DynamicJsonDocument& jsonData) {
  std::ofstream file(filePath);
  if (file.is_open()) {
    serializeJson(jsonData, file);
    file.close();
    return true;
  } else {
    return false;
  }
}