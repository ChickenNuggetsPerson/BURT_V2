#include <map>
#pragma once

int readFile(const char* fileName);

void writeFile(const char* fileName, int numToWrite);
void writeFile(const char* fileName, const char* content);

void appendFile(const char* fileName, int numToAppend);
void appendFile(const char* fileName, const char* content);

bool copyFile(const char* file, const char* dest);
bool fileExists(const char* name);


class Dfile {
    private:
        std::string name;
    public:
        Dfile();
        Dfile(const char* fileName) {
            name = std::string(fileName);
        }
        Dfile(std::string fileName) {
            name = fileName;
        }
        std::string getName() {
            return name;
        }
};
class Directory {
    private:
        std::string dirName;
        std::vector<Directory> childDirectories;
        std::vector<Dfile> childFiles;

        void addDFile(Dfile file) {
            childFiles.push_back(file);
        }
        void addDirectory(Directory dir) {
            childDirectories.push_back(dir);
        }
        std::vector<std::string> getFirstDir(std::string dirList) {
            size_t found = dirList.find('/');

            if (found != std::string::npos) {
                // Has directory
                std::vector<std::string> tmp;
                tmp.push_back(std::string(dirList.substr(0, found))); // Directory Name
                tmp.push_back(std::string(dirList.substr(found + 1))); // Rest of stuff
                return tmp;
            } else {
                // No directories
                std::vector<std::string> empty;
                return empty;
            }
        }
        int findDir(std::vector<Directory> vec, std::string target) {
            for (int i = 0; i < vec.size(); i++) {
                if (vec.at(i).getName() == target) {
                    return i;
                }
            }
            return -1;
        }
    public:
        Directory(std::string directoryName) {
            dirName = directoryName;
        }
        std::string getName() {
            return dirName;
        }
        void addFile(std::string path) {
            std::vector<std::string> info = getFirstDir(path);
            // Info Vector:
            //  0: Most Significant Directory Name
            //  1: Everything Else
            if (info.size() == 0) {
                // Is a file
                addDFile(Dfile(path));
            } else {
                // Is a Directory

                int result = findDir(childDirectories, info.at(0));
                if (result == -1) {
                    childDirectories.push_back(Directory(info.at(0)));
                    childDirectories.at(childDirectories.size() - 1).addFile(info.at(1));
                } else {
                    childDirectories.at(result).addFile(info.at(1));
                }
            }

        }
        std::vector<std::string> listEverything() {
            std::vector<std::string> everything;

            // Loop Through Directories
            for (int i = 0; i < childDirectories.size(); i++) {
                std::vector<std::string> tmp = childDirectories.at(i).listEverything();
                for (int i = 0; i < tmp.size(); i++) {
                    everything.push_back(dirName + std::string("/") + tmp.at(i));
                }
            }
            // Loop Through Files
            for (int i = 0; i < childFiles.size(); i++) {
                everything.push_back(dirName + std::string("/") + childFiles.at(i).getName());
            }
            
            return everything;
        }
        
};

Directory getAllDirs();


double degreeToRad(double degree);
double radToDegree(double rad);

double limitAngle(double angle);