// This file has been prepared for Doxygen automatic documentation generation.
//Seth M King

#include "comms.h"
#include <algorithm>
#include <cstdlib>

using std::fill;
using std::begin;
using std::end;
using std::for_each;
using std::clamp;
using std::stod;

Comms::Comms() : serial(USBTX, USBRX, BAUD), messageReceived(false)
{
    serial.attach(callback(this, &Comms::messageReceive));
    fill(begin(staticBuffer), end(staticBuffer), 0);
    counter = 0;
    div = static_cast<double>(DIVISOR);
    MAX_V = MAX_VELOCITY/div;
    MIN_V = MIN_VELOCITY/div;
}


void Comms::ProcessMessages()
{
    Parse();
}

void Comms::messageReceive()
{
    while(serial.readable())
    {
        auto c = static_cast<char>(serial.getc());
        staticBuffer[counter++] = c;
        if(c == END)
        {
            messageReceived = true;
            //counter = 0;
        }
        //serialBuffer.push_back(c);
    }
}


void Comms::Parse()
{

    if(findStart())
    {
        //Serial.println(readBytes);

        //Serial.print(serialBuffer);

        char command = serialBuffer[0];//move to comamand character
        serialBuffer.erase(begin(serialBuffer));
        switch(command)
        {
            case VELOCITY_TARGET:
                parseTargetVelocity();
                break;
            case VELOCITY:
                parseVelocity();
                break;
            case PWM:
                parsePWM();
                break;
            case GAINS:
                parseGains();
                break;
            case CURRENT:
                parseCurrent();
                break;
            case START:
                parseStart();
                break;
            case DIRECTION:
                parseDirection();
                break;
            default:
                break;
        }
        serialBuffer.clear();
        messageReceived = false;
    }
}

bool Comms::findStart()
{
    if(!messageReceived)
        return  false;

    //printf("Got message:%d\n", counter);
    //for_each(begin(staticBuffer), end(staticBuffer),[&](auto c){serial.putc(c);});

    bool found = false;
    for(auto& single:staticBuffer)
    {
        if(single == BEGINNING || found)
        {
            serialBuffer.push_back(single);
            found = true;
        }

        if(single == END && found)
        {
            serialBuffer.erase(begin(serialBuffer)); // remove B
            fill(begin(staticBuffer), end(staticBuffer), 0);
            counter = 0;
            return true;
        }
    }

    return false;
}

void Comms::Send(int data)
{
    serial.printf("B%04dQ\n",data);
}

void Comms::ReadorWrite(auto read, auto write)
{

    char Read = serialBuffer[0];
    serialBuffer.erase(begin(serialBuffer));

    if(Read == WRITE)
    {
        write();
    }
    read();
}

void Comms::parseTargetVelocity()
{
    auto read = [&](){Send((int)(localSerialdata.targetVelocity * MULTIPLIER));};
    auto write = [&]()
    {
        char velocity[2];

        velocity[0] = serialBuffer[0];
        velocity[1] = serialBuffer[1];
        localSerialdata.targetVelocity = clamp(atof(velocity)/div, MIN_V, MAX_V);
    };

    ReadorWrite(read, write);
}

void Comms::parseVelocity()
{
    Send((int)(localSerialdata.velocity * MULTIPLIER));
}

void Comms::parsePWM()
{
    Send(localSerialdata.controlPWM);
}

void Comms::parseGains()
{
    auto read = [&]()
    {
        Send((int)(localSerialdata.P_gain * MULTIPLIER));
        Send((int)(localSerialdata.I_gain * MULTIPLIER));
    };
    auto write = [&]()
    {

        string p_string(begin(serialBuffer),begin(serialBuffer)+GAIN_SIZE);
        string i_string(begin(serialBuffer)+GAIN_SIZE,begin(serialBuffer)+(2*GAIN_SIZE));

        localSerialdata.P_gain = stod(p_string)/MULTIPLIER;
        localSerialdata.I_gain = stod(i_string)/MULTIPLIER;

        localSerialdata.P_gain = clamp(localSerialdata.P_gain, 0.1, 10.0);
        localSerialdata.I_gain = clamp(localSerialdata.I_gain, 0.1, 10.0);
    };

    ReadorWrite(read, write);
}

void Comms::parseCurrent()
{
    Send((int)(localSerialdata.current * MULTIPLIER));
}

void Comms::parseStart()
{
    auto read = [&]()
    {
        Send((int)localSerialdata.started);
    };
    auto write = [&]()
    {
        localSerialdata.started = (serialBuffer[0] == '1');
    };

    ReadorWrite(read, write);
}

void Comms::parseDirection()
{
    auto read = [&]()
    {
        Send((int)localSerialdata.forward);
    };
    auto write = [&]()
    {
       localSerialdata.forward = (serialBuffer[0] == '1');
    };

    ReadorWrite(read, write);
}

void Comms::getData(DataTransport &data)
{
    dataMutex.lock();
    data = localSerialdata;
    dataMutex.unlock();
}

void Comms::setData(const DataTransport data)
{
    dataMutex.lock();
    localSerialdata = data;
    dataMutex.unlock();
}

