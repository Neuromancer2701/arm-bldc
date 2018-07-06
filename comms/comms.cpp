// This file has been prepared for Doxygen automatic documentation generation.
//Seth M King

#include "comms.h"
#include <algorithm>

using std::fill;
using std::begin;
using std::end;

Comms::Comms() : serial(USBTX, USBRX, BAUD), messageReceived(false)
{
    serial.attach(callback(this, &Comms::messageReceive));
    fill(begin(staticBuffer), end(staticBuffer), 0);
    counter = 0;
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
            counter = 0;
        }
        //serialBuffer.push_back(c);
    }
}


void Comms::Parse()
{

    if(messageReceived && findStart())
    {

        //Serial.print("readBytes: ");
        //Serial.println(readBytes);

        //Serial.print(serialBuffer);

        char command = serialBuffer[0];//move to comamand character
        serialBuffer.erase(begin(serialBuffer));
        switch(command)
        {
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
            return true;
        }
    }

    return false;
}

void Comms::Send(int data)
{
    serial.printf("B%04d\n",data);
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

void Comms::parseVelocity()
{
    auto read = [&](){Send((int)(data.velocity * MULTIPLIER));};
    auto write = [&]()
    {
        char velocity[2];

        velocity[0] = serialBuffer[0];
        velocity[1] = serialBuffer[1];
        data.targetVelocity = atof(velocity)/DIVISOR;

        if(data.targetVelocity > MAX_VELOCITY/(double)DIVISOR)
        {
            data.targetVelocity = MAX_VELOCITY/(double)DIVISOR;
        }

        if(data.targetVelocity < MIN_VELOCITY/(double)DIVISOR)
        {
            data.targetVelocity = MIN_VELOCITY/(double)DIVISOR;
        }
    };

    ReadorWrite(read, write);

}

void Comms::parsePWM()
{
    Send(data.controlPWM);
}

void Comms::parseGains()
{
    auto read = [&]()
    {
        Send((int)(data.P_gain * MULTIPLIER));
        Send((int)(data.I_gain * MULTIPLIER));
    };
    auto write = [&]()
    {

        string p_string(begin(serialBuffer),begin(serialBuffer)+GAIN_SIZE);
        string i_string(begin(serialBuffer)+GAIN_SIZE,begin(serialBuffer)+(2*GAIN_SIZE));

        data.P_gain = atof(p_string.c_str())/MULTIPLIER;
        data.I_gain = atof(i_string.c_str())/MULTIPLIER;
    };

    ReadorWrite(read, write);
}

void Comms::parseCurrent()
{
    Send((int)(data.current * MULTIPLIER));
}

void Comms::parseStart()
{
    auto read = [&]()
    {
        Send((int)data.started);
    };
    auto write = [&]()
    {
        //startMotor((serialBuffer[index] == '1'));
    };

    ReadorWrite(read, write);
}

void Comms::parseDirection()
{
    auto read = [&]()
    {
        Send((int)data.forward);
    };
    auto write = [&]()
    {
        //ChangeDirection((serialBuffer[index]== '1');
        int i = 1;
    };

    ReadorWrite(read, write);
}

const DataTransport &Comms::getData() const
{
    return data;
}

void Comms::setData(const DataTransport &data)
{
    Comms::data = data;
}

