

#include "bldc.h"
#include "utils.h"
#include <algorithm>


using std::make_unique;
using std::clamp;



BLDC::BLDC()://Phase_A(D11),Phase_B(D10),Phase_C(D9),
             LowSide(D5/*C_LOW*/, D6 /*B_LOW*/, D7 /*A_LOW*/),
             HallIO(D2/*HALL3*/, D3/*HALL2*/, D4/*HALL1*/)
{
    cycleCounter = 0;

    directionState = FORWARD;

    currentCommunationState = State6;
    newCommunationState =  State6;

    deltaT = 0;
    speedTimer.start();

    error = previousError = 0.0;

    communication.puts("Make PWM objects\n");

    for( auto& index:utils::range(FET_IO))
    {
        HighSide[index] = make_unique<PwmOut>(HighSide_pins[index]);
    }
    communication.puts("after PWM objects made!!!\n");

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

    //LaunchSerialThread();
}

void BLDC::ReadHalls()
{
    data.rawHalls = HallIO.read();
    newCommunationState = static_cast<commumationStates>(data.rawHalls);

}


void BLDC::Control()
{
    ReadHalls();
    if(data.started || directionState == CHANGING)
    {
        CalculateCommutationState();
    }
}

void BLDC::InputTest()
{

    data.controlPWM = 75;
    ReadHalls();
    currentCommunationState = newCommunationState;
    SetStateIO();

    wait_ms(100);
}

void BLDC::FullCycleTest()
{

    data.controlPWM = 75;
    data.forward = true;

    unsigned long Delay = 1000;

    currentCommunationState = State1;
    SetStateIO();
    wait_ms(Delay);

    /*
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
*/
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

    char buffer[256];
    sprintf(buffer,"hideIndex: %d Low: %02x PWM: %f\n", highindex, lowsetting, data.controlPWM/static_cast<float>(100.0));
    communication.puts(buffer);
    HighSide[highindex].get()->write(data.controlPWM/static_cast<float>(100.0));
    communication.puts("After PWM\n");
    LowSide.write(lowsetting);
    communication.puts("After IO\n");
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

void BLDC::LaunchSerialThread()
{
    serialThread.start(callback(BLDC::MonitorSerial, this));
}

void BLDC::MonitorSerial(void *commObject)

{
    BLDC* ptr = static_cast<BLDC*>(commObject);

    while(1)
    {
        ptr->communication.setData(ptr->data); // Update data transport to
        ptr->communication.ProcessMessages();
        ptr->communication.getData(ptr->data); // Update data transport from
    }
}

