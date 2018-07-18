
#include "bldc.h"
#include "utils.h"

using std::make_unique;

commumationStates startSeqeunce[BLDC::COMMUTATION_STATES] = {State1, State2, State3, State4, State5, State6};

BLDC::BLDC():LowSide(D7, D6, D5), HallIO(D3, D4, D5)
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
}

BLDC::~BLDC()
{

}

void BLDC::initPWM()
{
    communication.startup();
    // begin with 500 pwm frequency
   // Palatis::SoftPWM.begin(500);

    // print interrupt load for diagnostic purposes
    //Palatis::SoftPWM.printInterruptLoad();

    //Palatis::SoftPWM.allOff();
}

void BLDC::ReadHalls()
{
    //RawHallData[HALL1_INDEX] = digitalRead(HALL1);
    //RawHallData[HALL2_INDEX] = digitalRead(HALL2);
    //RawHallData[HALL3_INDEX] = digitalRead(HALL3);
    newCommunationState = (commumationStates)((RawHallData[HALL1_INDEX] << HALL1_SHIFT) | (RawHallData[HALL2_INDEX] << HALL2_SHIFT) | RawHallData[HALL3_INDEX]);
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
#if 0
    PORTB = 0x00; //clear io to give a bit of rest time between states. To prevent shoot through.
    Palatis::SoftPWM.allOff();

    int highSideIndex = 0;
    unsigned short lowSide = 0;
    switch(currentCommunationState)
		{

			case State1:
                 if(forward)
                 {
                     highSideIndex = AH_INDEX;
                     lowSide = CL_HIGH_PORTB;
                 }
                 else
                 {
                     highSideIndex = CH_INDEX;
                     lowSide = AL_HIGH_PORTB;
                 }
				 break;

			case State2:
                 if(forward)
                 {
                     highSideIndex = BH_INDEX;
                     lowSide = CL_HIGH_PORTB;
                 }
                 else
                 {
                     highSideIndex = CH_INDEX;
                     lowSide = BL_HIGH_PORTB;
                 }
                 break;

			case State3:
                 if(forward)
                 {
                     highSideIndex = BH_INDEX;
                     lowSide = AL_HIGH_PORTB;
                 }
                 else
                 {
                     highSideIndex = AH_INDEX;
                     lowSide = BL_HIGH_PORTB;
                 }
                 break;
			case State4:
                 if(forward)
                 {
                     highSideIndex = CH_INDEX;
                     lowSide = AL_HIGH_PORTB;
                 }
                 else
                 {
                     highSideIndex = AH_INDEX;
                     lowSide = CL_HIGH_PORTB;
                 }
                 break;
			case State5:
                 if(forward)
                 {
                     highSideIndex = CH_INDEX;
                     lowSide = BL_HIGH_PORTB;;
                 }
                 else
                 {
                     highSideIndex = BH_INDEX;
                     lowSide = CL_HIGH_PORTB;
                 }
                 break;
			case State6:
                if(forward)
                {
                    highSideIndex = AH_INDEX;
                    lowSide = BL_HIGH_PORTB;
                }
                else
                {
                    highSideIndex = BH_INDEX;
                    lowSide = AL_HIGH_PORTB;
                }
                break;

            default:
                PORTB = 0x00;
                Palatis::SoftPWM.allOff();
                break;

		}

    //char data[256];
    //sprintf(data,"hideIndex: %d PORTB: %02x Forward: %s", highSideIndex, lowSide, forward ? "true" : "false");
    //Serial.println(data);

    Palatis::SoftPWM.set(highSideIndex, controlPWM);
    PORTB = lowSide;
#endif
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
#if 0
    previousError = error;
    error = targetVelocity - velocity;
    directionWindow = millis() + SAMPLE_WINDOW_MS;

    double controlPWMStep = (P_gain * error) + (I_gain * ((previousError + error)/2));

    if(controlPWMStep > MAX_PWM_STEP)
        controlPWMStep = MAX_PWM_STEP;

    controlPWM += controlPWMStep;

    if( controlPWM > MAX_PWM )
        controlPWM = MAX_PWM;

    if( controlPWM < MIN_PWM )
        controlPWM = MIN_PWM;
#endif
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

