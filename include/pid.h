

struct PIDConfig {
    double P;
    double I;
    double D;

    PIDConfig();
    PIDConfig(double p, double i, double d);
};



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

        PID(PIDConfig config);
        PID(PIDConfig config, double desired);

        void setMax(double maxVal);
        void setMin(double minVal);

        double iterate(double newVal);
        double iterate(double newVal, double newDesired);
};