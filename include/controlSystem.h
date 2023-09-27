#pragma once;
#include "vex.h"
#include "vector"

namespace controlSystem {

    enum MovementType {
        noType,
        ToggleSpin,
        HoldSpin,
    };
    enum ControllerRef {
        noRef,
        btn_UP,
        btn_DOWN,
        btn_LEFT,
        btn_RIGHT,
        btn_A,
        btn_B,
        btn_X,
        btn_Y,
        btn_L1,
        btn_L2,
        btn_R1,
        btn_R2
    };

    struct ControlObject {
        vex::motor* mainMotorPtr = nullptr;
        vex::motor* altMotorPtr = nullptr;
        bool twoMotors = false;
        vex::directionType direction = vex::directionType::fwd;
        vex::directionType altDirection = vex::directionType::fwd;
        vex::rotationUnits units = vex::rotationUnits::deg;
        vex::velocityUnits velUnits = vex::velocityUnits::rpm;
        vex::brakeType brakes = vex::brakeType::coast;
        MovementType type = MovementType::noType;
        ControllerRef ref = ControllerRef::noRef;
        double primaryVal = 0.00; // True
        double secondaryVal = 0.00; // False
        bool toggleState = false;

    };

    class MotorController {
        private:
            vex::controller* controllerPtr;
            std::vector<ControlObject> objects;

            void applyMotor(ControlObject& obj);

            void setStates();
            bool R1State = false;
            bool R2State = false;
            bool L1State = false;
            bool L2State = false;
            bool UpState = false;
            bool DownState = false;
            bool LeftState = false;
            bool RightState = false;
            bool AState = false;
            bool BState = false;
            bool XState = false;
            bool YState = false;


        public:

            MotorController(vex::controller* controllerPtr);

            void addObject(ControlObject obj);

            void step();

    };

    class MotorHolder {
        private:

            vex::motor* motorPtr;
            pid::PID pidSystem;

            vex::rotationUnits units;

            bool hasNewVal = false;
            double newVal = 0.00;

            bool running = true;

        public:

            MotorHolder(vex::motor* ptr, pid::PIDConfig config, vex::rotationUnits measureUnits) {
                motorPtr = ptr;
                pidSystem.changeConfig(config);
                units = measureUnits;
            }

            void iterate() {
                if (!running) {return;}
                double power = 0.00;
                if (hasNewVal) {
                    power = pidSystem.iterate(motorPtr->position(units), newVal);
                    hasNewVal = false;
                } else {
                    power = pidSystem.iterate(motorPtr->position(units));
                }
            
                motorPtr->spin(vex::directionType::rev, power, vex::voltageUnits::volt);

            }

            double getRot() {
                motorPtr->position(units);
            }

            void setNewVal(double newVal) {
                this->newVal = newVal;
                hasNewVal = true;
            }
            void setRunning(bool newStatus) {
                if (newStatus) {
                    motorPtr->setBrake(vex::brakeType::coast);
                } else {
                    motorPtr->setBrake(vex::brakeType::brake);
                }
                running = newStatus;
            }
            bool getRunning() {
                return running;
            }
    };

    class CatapultSystem {
        private:
            vex::motor* motorPtr;

            double rotationOffset = NAN;
            double rotation = 0.00;
            bool running = false;

        public:
            CatapultSystem(vex::motor* ptr) {
                this->motorPtr = ptr;
            }

            void setSpeed(double volts) {
                if (this->running) { return; }
                this->motorPtr->spin(vex::directionType::fwd, volts, vex::voltageUnits::volt);
            }

            void reset() {
                //DEBUGLOG("Reset");

                this->running = true;
                this->motorPtr->spin(vex::directionType::fwd, 8, vex::voltageUnits::volt);
                int countDown = 200;
                while (this->motorPtr->velocity(vex::percentUnits::pct) > 50 || countDown > 0) {
                    //DEBUGLOG(this->motorPtr->velocity(vex::percentUnits::pct));
                    wait(3, vex::timeUnits::msec);
                    countDown--;
                }
                this->motorPtr->spin(vex::directionType::fwd, 0, vex::voltageUnits::volt);
                this->motorPtr->resetPosition();
                this->running = false;
            }
            void launch() {
                //DEBUGLOG("Launch");

                this->running = true;
                this->motorPtr->spin(vex::directionType::fwd, 10, vex::voltageUnits::volt);
                while ((int)this->motorPtr->position(vex::rotationUnits::deg) % 360 < 180) {
                    wait(3, vex::timeUnits::msec);
                }
                this->motorPtr->spin(vex::directionType::fwd, 0, vex::voltageUnits::volt);
                this->running = false;
            }
    };
};