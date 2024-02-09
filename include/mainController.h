#pragma once

enum WingStates {
    W_loose, W_pos1, W_pos2, W_pos3, W_close, W_CataAlign, W_LeftAutonPoleTouch, W_LeftAutonSwipeTriball
};

void startCatapult();
void stopCatapult();

void setMotors(double leftAmt, double rightAmt, vex::velocityUnits units);
void setMotors(double leftAmt, double rightAmt, vex::voltageUnits units);



class DriveSystem {

    private:

        vex::brain* brainPtr;

        double mainVoltage = 0.00;
        double maxVoltage = 0.00;
        
        double maxDriveVolt = 0.00;

        misc::ValueAverager<50> speedAverage = misc::ValueAverager<50>();

        inline double lerp(double start, double end, double amt) { return start + ((end - start) * amt); };

    public:

        DriveSystem(vex::brain* brainPtr, double mainVoltage, double maxVoltage) : brainPtr(brainPtr), mainVoltage(mainVoltage), maxVoltage(maxVoltage) {};

        void iterate(double leftSpeed, double rightSpeed) {
            double avg = fabs(speedAverage.iterate( (leftSpeed / 2) + (rightSpeed / 2) )) / 100.0;
            maxDriveVolt = lerp(mainVoltage, maxVoltage, avg);

            setMotors(
                lerp(0, maxDriveVolt, leftSpeed / 100.00), 
                lerp(0, maxDriveVolt, rightSpeed / 100.00), 
                vex::voltageUnits::volt
            );
        };

        inline double getMaxVoltage() { return maxDriveVolt; }

};
extern DriveSystem driveSystem;


int controllerTask();
void mainControllerRender();
void mainControllerMessage(const char* text, int timeout);
bool mainControllerOverlay(const char* question, const char* trueOption, const char* falseOption);
int mainControllerPickOption(std::vector <const char*> options);