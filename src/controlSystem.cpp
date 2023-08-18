#include "robotConfig.h"
#include "controlSystem.h"

using namespace controlSystem;

MotorController::MotorController(vex::controller* controllerPtr) {
    this->controllerPtr = controllerPtr;
}
void MotorController::setStates() {
    R1State = controllerPtr->ButtonR1.pressing();
    R2State = controllerPtr->ButtonR2.pressing();
    L1State = controllerPtr->ButtonL1.pressing();
    L2State = controllerPtr->ButtonL2.pressing();
    UpState = controllerPtr->ButtonUp.pressing();
    DownState = controllerPtr->ButtonDown.pressing();
    LeftState = controllerPtr->ButtonLeft.pressing();
    RightState = controllerPtr->ButtonRight.pressing();
    AState = controllerPtr->ButtonA.pressing();
    BState = controllerPtr->ButtonB.pressing();
    XState = controllerPtr->ButtonX.pressing();
    YState = controllerPtr->ButtonY.pressing();
};
void MotorController::addObject(ControlObject obj) {
    if (obj.twoMotors) {
        obj.altMotorPtr->setBrake(obj.brakes);
        obj.altMotorPtr->spin(obj.altDirection, 0, voltageUnits::volt);
    }
    obj.mainMotorPtr->setBrake(obj.brakes);
    obj.mainMotorPtr->spin(obj.direction, 0, voltageUnits::volt);
    
    objects.push_back(obj);
};
void MotorController::step() {
    if (!controllerPtr->installed()) {return;}
    for (auto& obj: objects) {
        applyMotor(obj);
    }
};
void MotorController::applyMotor(ControlObject& obj) {
    
    bool stateChanged = false;
    bool newState = false;
    
    switch (obj.ref) {
        case noRef: {
            break;
        };
        case btn_UP: {
            newState = controllerPtr->ButtonUp.pressing();
            if (UpState != newState) {
                stateChanged = true;
            }
            UpState = newState;
            break;
        };
        case btn_DOWN: {
            newState = controllerPtr->ButtonDown.pressing();
            if (DownState != newState) {
                stateChanged = true;
            }
            DownState = newState;
            break;
        };
        case btn_LEFT: {
            newState = controllerPtr->ButtonLeft.pressing();
            if (LeftState != newState) {
                stateChanged = true;
            }
            LeftState = newState;
            break;
        };
        case btn_RIGHT: {
            newState = controllerPtr->ButtonRight.pressing();
            if (RightState != newState) {
                stateChanged = true;
            }
            RightState = newState;
            break;
        };
        case btn_A: {
            newState = controllerPtr->ButtonA.pressing();
            if (AState != newState) {
                stateChanged = true;
            }
            AState = newState;
            break;
        };
        case btn_B: {
            newState = controllerPtr->ButtonB.pressing();
            if (BState != newState) {
                stateChanged = true;
            }
            BState = newState;
            break;
        };
        case btn_X: {
            newState = controllerPtr->ButtonX.pressing();
            if (XState != newState) {
                stateChanged = true;
            }
            XState = newState;
            break;
        };
        case btn_Y: {
            newState = controllerPtr->ButtonY.pressing();
            if (YState != newState) {
                stateChanged = true;
            }
            YState = newState;
            break;
        };
        case btn_L1: {
            newState = controllerPtr->ButtonL1.pressing();
            if (L1State != newState) {
                stateChanged = true;
            }
            L1State = newState;
            break;
        };
        case btn_L2: {
            newState = controllerPtr->ButtonL2.pressing();
            if (L2State != newState) {
                stateChanged = true;
            }
            L2State = newState;
            break;
        };
        case btn_R1: {
            newState = controllerPtr->ButtonR1.pressing();
            if (R1State != newState) {
                stateChanged = true;
            }
            R1State = newState;
            break;
        };
        case btn_R2: {
            newState = controllerPtr->ButtonR2.pressing();
            if (R2State != newState) {
                stateChanged = true;
            }
            R2State = newState;
            break;
        };

    }
    
    if (!stateChanged) {return;}
    switch (obj.type) {
        case noType: {
            break;
        };
        case ToggleSpin: {
            if (newState) {
                obj.toggleState = !obj.toggleState;
            }
            obj.mainMotorPtr->spin(obj.direction, obj.toggleState ? obj.primaryVal : obj.secondaryVal, obj.velUnits);
            if (obj.twoMotors) {
                obj.altMotorPtr->spin(obj.altDirection, obj.toggleState ? obj.primaryVal : obj.secondaryVal, obj.velUnits);
            }
            break;
        };
        case ToggleAngle: {
            if (newState) {
                obj.toggleState = !obj.toggleState;
            }

            if (obj.toggleState) {
                obj.mainMotorPtr->spinTo(obj.primaryVal, obj.units, false);
            } else {
                obj.mainMotorPtr->spinTo(obj.secondaryVal, obj.units, false);
            }
            if (obj.twoMotors) {
                if (obj.toggleState) {
                    obj.altMotorPtr->spinTo(obj.primaryVal, obj.units, false);
                } else {
                    obj.altMotorPtr->spinTo(obj.secondaryVal, obj.units, false);
                }
            }

            break;
        };
        case HoldSpin: {
            
            obj.mainMotorPtr->spin(obj.direction, newState ? obj.primaryVal : obj.secondaryVal, obj.velUnits);
            if (obj.twoMotors) {
                obj.mainMotorPtr->spin(obj.altDirection, newState ? obj.primaryVal : obj.secondaryVal, obj.velUnits);    
            }

            break;
        };
        case HoldAngle: {

            obj.mainMotorPtr->spinTo(obj.primaryVal, obj.units, false);
            if (obj.twoMotors) {
                obj.altMotorPtr->spinTo(obj.primaryVal, obj.units, false);
            }
            
            break;
        };
    };
    
    

};