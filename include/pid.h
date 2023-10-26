#pragma once

// Usefull resouce for PID tunning 
// https://www.thorlabs.com/newgrouppage9.cfm?objectgroup_id=9013#:~:text=To%20tune%20your%20PID%20controller,to%20roughly%20half%20this%20value.

namespace pid {
    
    struct PIDConfig {
        double P;
        double I;
        double D;

        PIDConfig();
        PIDConfig(double p, double i, double d);
    };

    // Define the PID class
    // By using this class, PIDs can be easily created and tuned for different scenarios
    class PID {

        private:

            double P = 0.0;
            double I = 0.0;
            double D = 0.0;

            int error;
            int prevError = 0;
            int derivative;
            int totalError = 0;

            double desiredVal = 0.00;


            bool hasMax = false;
            bool hasMin = false;
            double max;
            double min;


        public:

            PID() {};
            PID(PIDConfig config);
            PID(PIDConfig config, double desired);

            void changeConfig(PIDConfig newConfig);

            void setMax(double maxVal);
            void setMin(double minVal);

            double iterate(double newVal);
            double iterate(double newVal, double newDesired);

            double getSet() {
                return desiredVal;
            };
    };
}
