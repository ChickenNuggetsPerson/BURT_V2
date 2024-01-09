#pragma once;
#include "vex.h"
#include "vector"

/*

Mostly redundant code. Refer to stateMachine.h for updated code 

*/



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

            double desiredVal = 0.00;

            bool running = true;

        public:

            MotorHolder(vex::motor* ptr, pid::PIDConfig config, vex::rotationUnits measureUnits) {
                this->motorPtr = ptr;
                this->pidSystem.changeConfig(config);
                this->units = measureUnits;
            }

            void calibrate() {
                this->setRunning(false);
                motorPtr->setPosition(-83, units);
                this->setRunning(true);
                this->setNewVal(210);
                wait(0.5, vex::timeUnits::sec);
                this->setRunning(false);
            }

            void iterate() {

                // DEBUGLOG(motorPtr->position(units));

                // 152 horz
                // 213 Calb

                if (!running) {return;}

                double power = 0.00;

                power = pidSystem.iterate(motorPtr->position(units), desiredVal);
                motorPtr->spin(vex::directionType::rev, power, vex::voltageUnits::volt);

                //DEBUGLOG(motorPtr->position(units), " ", power, " ", pidSystem.getSet());

            }

            double getRot() {
                motorPtr->position(units);
            }

            void setNewVal(double newVal) {
                this->desiredVal = newVal;
            }
            void setRunning(bool newStatus) {

                DEBUGLOG("NEW ARM RUNNING: ", newStatus ? "TRUE" : "FALSE");

                if (newStatus) {
                    motorPtr->setBrake(vex::brakeType::coast);
                } else {
                    motorPtr->setBrake(vex::brakeType::coast);
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

            vex::controller* controllerPtr;

        public:
            CatapultSystem(vex::motor* ptr, vex::controller* controllerPtr) {
                this->motorPtr = ptr;
                this->controllerPtr = controllerPtr;
            }

            void setSpeed(double volts) {
                if (this->running) { return; }
                this->motorPtr->spin(vex::directionType::fwd, volts, vex::voltageUnits::volt);
            }

            void reset() {
                // DEBUGLOG("Reset");

                if (this->running) { return; }

                this->running = true;
                this->motorPtr->spin(vex::directionType::fwd, 8, vex::voltageUnits::volt);
                int countDown = 100;
                while (this->motorPtr->torque(vex::torqueUnits::Nm) < 0.3 || countDown > 0) {
                    wait(3, vex::timeUnits::msec);
                    countDown--;

                    if (controllerPtr->ButtonRight.pressing()) {
                        this->running = false;
                        this->motorPtr->spin(vex::directionType::fwd, 0, vex::voltageUnits::volt);
                        return;        
                    }
                }
                this->motorPtr->spin(vex::directionType::fwd, 0, vex::voltageUnits::volt);
                this->motorPtr->resetPosition();
                this->running = false;
            }
            void launch() {
                if (this->running) { return; }

                this->running = true;
                this->motorPtr->spin(vex::directionType::fwd, 12, vex::voltageUnits::volt);
                while ((int)this->motorPtr->position(vex::rotationUnits::deg) % 360 < 180) {
                    wait(3, vex::timeUnits::msec);

                    if (controllerPtr->ButtonRight.pressing()) {
                        this->running = false;
                        this->motorPtr->spin(vex::directionType::fwd, 0, vex::voltageUnits::volt);
                        return;        
                    }
                }
                this->motorPtr->spin(vex::directionType::fwd, 0, vex::voltageUnits::volt);
                this->running = false;
            }

            void holdDown() {
                if (this->running) { return; }

                this->running = true;


                this->motorPtr->spin(vex::directionType::rev, 8, vex::voltageUnits::volt);
                while (fabs(this->motorPtr->torque(vex::torqueUnits::Nm)) < 1) {
                    wait(3, vex::timeUnits::msec);
                    
                    if (controllerPtr->ButtonRight.pressing()) {
                        this->running = false;
                        this->motorPtr->spin(vex::directionType::fwd, 0, vex::voltageUnits::volt);
                        return;        
                    }
                }
                this->motorPtr->spin(vex::directionType::fwd, 0, vex::voltageUnits::volt);


                wait(100, vex::timeUnits::msec);


                this->motorPtr->setPosition(0, vex::rotationUnits::deg);
                this->motorPtr->spin(vex::directionType::fwd, 8, vex::voltageUnits::volt);
                while ((int)this->motorPtr->position(vex::rotationUnits::deg) < 366) {
                    wait(1, vex::timeUnits::msec);

                    if (controllerPtr->ButtonRight.pressing()) {
                        this->running = false;
                        this->motorPtr->spin(vex::directionType::fwd, 0, vex::voltageUnits::volt);
                        return;        
                    }
                }

                this->motorPtr->spin(vex::directionType::fwd, 0, vex::voltageUnits::volt);
                this->running = false;
            }

    };
};