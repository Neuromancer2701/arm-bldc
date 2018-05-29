// This file has been prepared for Doxygen automatic documentation generation.
//Seth M King

#include "comms.h"

Comms::Comms() : serial(USBTX, USBRX), messageReceived(false)
{
    serial.baud(BAUD);
    serial.attach(this,&Comms::messageReceive, MODSERIAL::RxAutoDetect);
    serial.autoDetectChar('\n');
}


void Comms::ProcessMessages()
{
    Parse();
}

void Comms::messageReceive(MODSERIAL_IRQ_INFO *q)
{
    MODSERIAL *sys = q->serial;
    serialBuffer.resize(BUFFER_SIZE);
    sys->move(serialBuffer.data(), BUFFER_SIZE);
    messageReceived = true;
    return 0;
}


void Comms::Parse()
{

    if(messageReceived && findStart())
    {

        //Serial.print("readBytes: ");
        //Serial.println(readBytes);

        //Serial.print(serialBuffer);

        unsigned char command = serialBuffer[1];//move to comamand character
        switch(command)
        {
            case VELOCITY:
                parseVelocity(index);
                break;
            case PWM:
                parsePWM();
                break;
            case GAINS:
                parseGains(index);
                break;
            case CURRENT:
                parseCurrent();
                break;
            case START:
                parseStart(index);
                break;
            case DIRECTION:
                parseDirection(index);
                break;
            default:
                break;
        }
    }
}

bool Comms::findStart()
{
    int counter = 0;
    for(for auto& single:serialBuffer)
    {
        if(single == BEGINNING)
        {
            serialBuffer.erase(begin(serialBuffer), begin(serialBuffer) + counter);
            return  true;
        }
        counter++;
    }

    return false;
}

void Comms::Send(int data)
{
    serial.printf("B%04d\n",data);
}

void Comms::parseVelocity(int index)
{
    index++;  // moved to read/write character
    if(serialBuffer[index] == READ)
    {
        Send((int)(velocity * MULTIPLIER));
    }
    else if(serialBuffer[index] == WRITE)
    {
        char velocity[2];
        index++;
        velocity[0] = serialBuffer[index];
        index++;
        velocity[1] = serialBuffer[index];
        targetVelocity = atof(velocity)/DIVISOR;

        if(targetVelocity > MAX_VELOCITY/(double)DIVISOR)
        {
            targetVelocity = MAX_VELOCITY/(double)DIVISOR;
        }

        if(targetVelocity < MIN_VELOCITY/(double)DIVISOR)
        {
            targetVelocity = MIN_VELOCITY/(double)DIVISOR;
        }



        Send((int)(targetVelocity * DIVISOR));
    }
}

void Comms::parsePWM()
{
    Send(controlPWM);
}

void Comms::parseGains(int index)
{
    index++;  // moved to read/write character
    if(serialBuffer[index] == READ)
    {
        Send((int)(P_gain * MULTIPLIER));
        Send((int)(I_gain * MULTIPLIER));
    }
    else if(serialBuffer[index] == WRITE)
    {
        index++;
        char p_string[4];
        char i_string[4];

        strncpy(p_string,(const char *)&serialBuffer[index], GAIN_SIZE);
        index += GAIN_SIZE;
        strncpy(i_string,(const char *)&serialBuffer[index], GAIN_SIZE);
        P_gain = atof(p_string)/MULTIPLIER;
        I_gain = atof(i_string)/MULTIPLIER;

        Send((int)(P_gain * MULTIPLIER));
        Send((int)(I_gain * MULTIPLIER));
    }
}

void Comms::parseCurrent()
{
    Send((int)(current * MULTIPLIER));
}

void Comms::parseStart(int index)
{
    index++;  // moved to read/write character
    if(serialBuffer[index] == READ)
    {
        Send((int)started);
    }
    else if(serialBuffer[index] == WRITE)
    {
        index++; // moved to started value
        startMotor((serialBuffer[index] == '1'));
        Send((int)started);
    }
}

void Comms::parseDirection(int index)
{
    index++;  // moved to read/write character
    if(serialBuffer[index] == READ)
    {
        Send((int)forward);
    }
    else if(serialBuffer[index] == WRITE)
    {
        index++; // moved to started value
        ChangeDirection((serialBuffer[index]== '1'));
        Send((int)forward);
    }
}

