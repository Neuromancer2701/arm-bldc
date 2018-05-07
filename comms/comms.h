// This file has been prepared for Doxygen automatic documentation generation.
//Seth M King

#ifndef __COMMS_H__
#define __COMMS_H__

#include <vector>

class Comms {
public:
    Comms();

    virtual ~Comms();
    void ProcessMessages();
    
private:
    std::vector<char> serialBuffer;

    void Parse();
    int findStart(int size);
    void parseVelocity(int index);
    void parsePWM();
    void parseGains(int index);
    void parseCurrent();
    void parseStart(int index);
    void parseDirection(int index)
    
}
    