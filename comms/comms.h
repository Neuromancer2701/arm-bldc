// This file has been prepared for Doxygen automatic documentation generation.
//Seth M King

#ifndef __COMMS_H__
#define __COMMS_H__

#include <vector>
#include <Serial.h>

class Comms {
public:
    Comms();

    virtual ~Comms();
    void ProcessMessages();
    
private:
    std::vector<char> serialBuffer;
    Serial          serial(USBTX, USBRX);

    void Parse();
    int findStart(int size);
    void parseVelocity(int index);
    void parsePWM();
    void parseGains(int index);
    void parseCurrent();
    void parseStart(int index);
    void parseDirection(int index);

    enum serialConstants
    {
        BEGINNING = 'B',
        VELOCITY  = 'V',
        PWM       = 'P',
        GAINS     = 'G',
        CURRENT   = 'C',
        START     = 'S',
        DIRECTION = 'D',
        READ      = 'R',
        WRITE     = 'W',
        END       = '\n',
        MIN_SIZE  = 3,
        BUFFER_SIZE = 16,
        GAIN_SIZE = 4

    };
    
}
    