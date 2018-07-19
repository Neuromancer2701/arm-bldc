

#include "bldc.h"
#include "utils.h"
#include <algorithm>


using std::make_unique;
using std::clamp;



BLDC::BLDC():LowSide(static_cast<PinName>(C_LOW), static_cast<PinName>(B_LOW), static_cast<PinName>(C_LOW)),
             HallIO(static_cast<PinName>(HALL3), static_cast<PinName>(HALL2), static_cast<PinName>(HALL3))
{
    cycleCounter = 0;

    directionState = FORWARD;

    currentCommunationState = State6;
    newCommunationState =  State6;

    previousTime = 0;
    currentTime = 0;
    error = previousError = 0.0;
    communication.setData(data);

    for( auto& index:utils::range(FET_IO))
    {
        HighSide[index] = make_unique<PwmOut>(HighSide_pins[index]);
    }

    for( auto& index:utils::range(COMMUTATION_STATES))
    {
        commutationMap.emplace(forwardSequence[index], make_pair(HighSideStates[index],LowSideStates[index]));
        forward2Reverse.emplace(forwardSequence[index],reverseSequence[index]);
    }
}

BLDC::~BLDC()
{

}

void BLDC::initPWM()
{
    communication.startup();

    for( auto& index:utils::range(FET_IO))
    {
        HighSide[index].get()->period_ms(PWM_PERIOD_MS);
        HighSide[index].get()->write(0.0);
    }

}

void BLDC::ReadHalls()
{
    newCommunationState = static_cast<commumationStates>(HallIO.read());
}


void BLDC::Control()
{
#if 0
    ReadHalls();
    if(started || directionState == CHANGING)
    {
        CalculateCommutationState();
    }
#endif
    communication.ProcessMessages();
}

void BLDC::InputTest()
{
#if 0
    controlPWM = 50;
    ReadHalls();
    currentCommunationState = newCommunationState;
    SetStateIO();

    delay(100);
#endif
}

void BLDC::FullCycleTest()
{
#if 0
    controlPWM = 50;
    unsigned long Delay = 1000;

    forward = true;

    currentCommunationState = State1;
    SetStateIO();
    delay(Delay);

    currentCommunationState = State2;
    SetStateIO();
    delay(Delay);

    currentCommunationState = State3;
    SetStateIO();
    delay(Delay);

    currentCommunationState = State4;
    SetStateIO();
    delay(Delay);

    currentCommunationState = State5;
    SetStateIO();
    delay(Delay);

    currentCommunationState = State6;
    SetStateIO();
    delay(Delay);
#endif
}


void BLDC::CalculateCommutationState()
{
#if 0
    if(newCommunationState == currentCommunationState)
    {
        previousTime = currentTime;
        currentTime = millis();

        if((directionState == CHANGING) && currentTime > directionWindow)
        {
            directionState  = forward ? FORWARD : REVERSE;
            startMotor(true);
        }

        return;
    }
    else
    {
        currentCommunationState = newCommunationState;
        cycleCounter++;

        previousTime = currentTime;
        currentTime = millis();
        velocity = TWO_PI * (RADIUS/(double)1000) * ((1/(double)CYCLES_PER_REV)/((currentTime - previousTime)/(double)1000));

        PORTB = 0x00; //clear io to give a bit of rest time between states. To prevent shoot through.
        Palatis::SoftPWM.allOff();

        if(directionState != CHANGING)  // changing direction do not calculate PWM
        {
            CalculatePWM();
        }
        else
        {
            return;
        }
    }

    //sprintf(data,"state: %d cycle count: %d velocity: %05d", currentCommunationState, cycleCounter,(int)(velocity * 1000));
    SetStateIO();
#endif
}

void BLDC::SetStateIO()
{

    LowSide.write(0x00);
    for_each(begin(HighSide),end(HighSide),[](auto pwm){pwm.get()->write(0.0);});  //Clear all PWMs

    auto& [highindex, lowsetting] = commutationMap[currentCommunationState];

    //char data[256];
    //sprintf(data,"hideIndex: %d PORTB: %02x Forward: %s", highSideIndex, lowSide, forward ? "true" : "false");
    //Serial.println(data);
    HighSide[highindex].get()->write(data.controlPWM);
    LowSide.write(lowsetting);
}


int *BLDC::getRawHallData()
{
    return (int *)RawHallData;
}


void BLDC::startMotor(bool start)
{
    #if 0

    if(start)
    {
        started = true;
        ReadHalls();
        currentCommunationState = startSeqeunce[(findIndex(newCommunationState) + 1) % COMMUTATION_STATES]; // make sure the two starting states are different.
    }
    else
    {
        currentCommunationState = newCommunationState = State1;
        controlPWM = DUTY_STOP;
        started = false;
    }
#endif
}

int BLDC::findIndex(commumationStates state)
{
#if 0
    for(int i = 0;i < COMMUTATION_STATES; i++)
    {
        if(startSeqeunce[i] == state)
            return i;
    }

#endif
return 1;
}



void BLDC::CalculatePWM()
{

    previousError = error;
    error = data.targetVelocity - data.velocity;
    directionWindow = millis() + SAMPLE_WINDOW_MS;

    double controlPWMStep = (data.P_gain * error) + (data.I_gain * ((previousError + error)/2));

    controlPWMStep = clamp(controlPWMStep, MAX_PWM_STEP/-1.0, MAX_PWM_STEP/1.0);

    data.controlPWM = clamp(data.controlPWM + static_cast<unsigned char>(controlPWMStep),MIN_PWM);

}

void BLDC::ChangeDirection(bool _forward)
{
#if 0
    if(_forward == forward)
        return;

    startMotor(false);
    directionState = CHANGING;
    forward = _forward;
#endif
}

