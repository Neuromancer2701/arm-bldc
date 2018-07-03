//
// Created by Seth on 5/31/18.
//

#ifndef ARM_BLDC_DATATRANSPORT_H
#define ARM_BLDC_DATATRANSPORT_H


class DataTransport {

public:
    bool forward;
    bool started;
    double velocity;
    double P_gain;
    double I_gain;
    double targetVelocity;
    unsigned char controlPWM;
    double current;

    DataTransport()
    {
        forward = true;
        started = false;
        velocity = 0.0;
        P_gain = 1.0;
        I_gain = 1.0;
        targetVelocity = 0.0;
        controlPWM = 0;
        current = 0.0;
    }
};


#endif //ARM_BLDC_DATATRANSPORT_H
