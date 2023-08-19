#pragma once

#include "vex.h"
#include "robotConfig.h"

namespace debugSystem {

    class Debugger {
        private:
            bool sendingMessages = false;
            const char* dataPrefix = "__ ";

        public:

            Debugger() {};
            void init() {
                sendingMessages = (misc::readFile(std::string(systemConfigFolder + systemEnableWSDebug).c_str()) == 1);
            };
            inline bool isSending() { return sendingMessages; }
            
            template<typename T>
            inline void sendData(const char* id, T data) { std::cout << dataPrefix << id << " " << data << std::endl;}
    };    
};