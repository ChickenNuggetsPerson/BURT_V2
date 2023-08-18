#pragma once;
#include "vex.h"
#include "vector"

namespace controlSystem {

    enum MovementType {
        noType,
        ToggleSpin,
        ToggleAngle,
        HoldSpin,
        HoldAngle,
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
};