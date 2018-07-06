// This file has been prepared for Doxygen automatic documentation generation.
//Seth M King

#ifndef __COMMS_H__
#define __COMMS_H__

#include <vector>
#include <array>
#include <functional>
#include "DataTransport.h"
#include "mbed.h"

using std::vector;
using std::function;
using std::array;

class Comms {
public:
    Comms();

    virtual ~Comms() = default;
    void ProcessMessages();
    const DataTransport &getData() const;
    void setData(const DataTransport &data);
    
private:

    void Parse();
    bool findStart();
    void parseVelocity();
    void parsePWM();
    void parseGains();
    void parseCurrent();
    void parseStart();
    void parseDirection();
    void messageReceive();
    void Send(int data);


    void ReadorWrite(auto read, auto write);

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
        GAIN_SIZE = 4,
        BAUD  = 115200,
        DIVISOR = 10,
        MAX_VELOCITY = 50,     	// divide by 10 m/sec
        MIN_VELOCITY = 5,		// divide by 10 m/sec
        MULTIPLIER = 100,

    };
    //BV35

    vector<char> serialBuffer;
    array<char, BUFFER_SIZE>  staticBuffer;
    unsigned char counter;
    RawSerial         serial;
    bool messageReceived;
    DataTransport data;

    
};
#endif