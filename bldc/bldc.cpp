

#include "bldc.h"
#include "utils.h"
#include <algorithm>


using std::make_unique;
using std::clamp;



BLDC::BLDC():LowSide(D5/*C_LOW*/, D6 /*B_LOW*/, D7 /*A_LOW*/),
             HallIO(D2/*HALL3*/, D3/*HALL2*/, D4/*HALL1*/)
{
    cycleCounter = 0;

    directionState = FORWARD;

    currentCommunationState = State6;
    newCommunationState =  State6;

    deltaT = 0;
    speedTimer.start();

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

    ReadHalls();
    if(data.started || directionState == CHANGING)
    {
        CalculateCommutationState();
    }

    communication.ProcessMessages();
}

void BLDC::InputTest()
{

    data.controlPWM = 50;
    ReadHalls();
    currentCommunationState = newCommunationState;
    SetStateIO();

    wait_ms(100);
}

void BLDC::FullCycleTest()
{

    data.controlPWM = 50;
    unsigned long Delay = 1000;

    data.forward = true;

    currentCommunationState = State1;
    SetStateIO();
    wait_ms(Delay);

    currentCommunationState = State2;
    SetStateIO();
    wait_ms(Delay);

    currentCommunationState = State3;
    SetStateIO();
    wait_ms(Delay);

    currentCommunationState = State4;
    SetStateIO();
    wait_ms(Delay);

    currentCommunationState = State5;
    SetStateIO();
    wait_ms(Delay);

    currentCommunationState = State6;
    SetStateIO();
    wait_ms(Delay);

}


void BLDC::CalculateCommutationState()
{

    if(newCommunationState == currentCommunationState)
    {
        if((directionState == CHANGING) && speedTimer.read_ms() > directionWindow)
        {
            directionState  = data.forward ? FORWARD : REVERSE;
            startMotor(true);
        }

        return;
    }
    else
    {
        currentCommunationState = newCommunationState;
        cycleCounter++;

        delta_T();
        data.velocity = TWO_PI * (RADIUS/(double)1000) * ((1/(double)CYCLES_PER_REV)/((deltaT)/(double)1000));

        LowSide.write(0x00);
        clearAllPwm();

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
}

void BLDC::SetStateIO()
{

    LowSide.write(0x00);
    clearAllPwm();

    auto& [highindex, lowsetting] = commutationMap[currentCommunationState];

    //char data[256];
    //sprintf(data,"hideIndex: %d PORTB: %02x Forward: %s", highSideIndex, lowSide, forward ? "true" : "false");
    //Serial.println(data);
    HighSide[highindex].get()->write(data.controlPWM);
    LowSide.write(lowsetting);
}

void BLDC::startMotor(bool start)
{

    if(start)
    {
        data.started = true;
        ReadHalls();
        nextState(); // make sure the two starting states are different.
    }
    else
    {
        currentCommunationState = newCommunationState = State1;
        data.controlPWM = DUTY_STOP;
        data.started = false;
    }
}

void BLDC::nextState()
{
    if(newCommunationState == State6)
    {
        currentCommunationState = State1;
    }
    else
    {
        auto pos = distance(begin(forwardSequence), find(begin(forwardSequence), end(forwardSequence), newCommunationState)) + 1;  //next position
        currentCommunationState =  forwardSequence[pos];
    }


    if(!data.forward)  //reverse
    {
        currentCommunationState = forward2Reverse[currentCommunationState];
    }
}



void BLDC::CalculatePWM()
{

    previousError = error;
    error = data.targetVelocity - data.velocity;
    directionWindow = speedTimer.read_ms() + SAMPLE_WINDOW_MS;

    double controlPWMStep = (data.P_gain * error) + (data.I_gain * ((previousError + error)/2));

    controlPWMStep = clamp(controlPWMStep, static_cast<double >(MAX_PWM_STEP/-1.0), static_cast<double >(MAX_PWM_STEP/1.0));

    data.controlPWM = clamp(data.controlPWM + static_cast<unsigned char>(controlPWMStep),static_cast<int>(MIN_PWM), static_cast<int>(MAX_PWM));

}

void BLDC::ChangeDirection(bool _forward)
{

    if(_forward == data.forward)
        return;

    startMotor(false);
    directionState = CHANGING;
    data.forward = _forward;

}

