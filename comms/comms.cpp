// This file has been prepared for Doxygen automatic documentation generation.
//Seth M King

#include "comms.h"

void Comms::ProcessMessages()
{
    Parse();
}


void Comms::Parse()
{
    int readBytes = Serial.available();
    if(readBytes >= MIN_SIZE)
    {

        //Serial.print("readBytes: ");
        //Serial.println(readBytes);

        if(readBytes > BUFFER_SIZE)
            readBytes = BUFFER_SIZE;
        if(Serial.readBytesUntil(END, serialBuffer, BUFFER_SIZE) > 0)
        {
            int index = findStart(readBytes);

            //Serial.print(serialBuffer);

            if(index >= 0)
            {
                index++;  //move to comamand character
                unsigned char command = serialBuffer[index];
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
    }

}

int Comms::findStart(int size)
{
    for(int i = 0; i < size;i++)
    {
        if(serialBuffer[i] == BEGINNING)
            return i;
    }

    return -1;
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

void Comms::Send(int data)
{
    char sendBuffer[BUFFER_SIZE];
    snprintf(sendBuffer,BUFFER_SIZE,"B%d\n",data);
    Serial.print(sendBuffer);
    Serial.flush();
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