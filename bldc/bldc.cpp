#include "Arduino.h"
#include "bldc.h"
#include "SoftPWM.h"


SOFTPWM_DEFINE_CHANNEL(AH_INDEX, DDRD, PORTD, PORTD4);  //Arduino pin 4
SOFTPWM_DEFINE_CHANNEL(BH_INDEX, DDRB, PORTB, PORTB1);  //Arduino pin 9
SOFTPWM_DEFINE_CHANNEL(CH_INDEX, DDRD, PORTD, PORTD5);  //Arduino pin 5

SOFTPWM_DEFINE_OBJECT_WITH_PWM_LEVELS(3, 100);
SOFTPWM_DEFINE_EXTERN_OBJECT_WITH_PWM_LEVELS(3, 100);

commumationStates startSeqeunce[BLDC::COMMUTATION_STATES] = {State1, State2, State3, State4, State5, State6};

BLDC::BLDC()
{
    cycleCounter = 0;
    forward = true;
    directionState = FORWARD;
    started = false;
    currentCommunationState = State6;
    newCommunationState =  State6;
    velocity = 0.0;
    previousTime = 0;
    currentTime = 0;

    P_gain = 1.0;
    I_gain = 1.0;
    targetVelocity = 0.0;
    error = previousError = 0.0;
    controlPWM = 0;

    pinMode(HALL1, INPUT);
    pinMode(HALL2, INPUT);
    pinMode(HALL3, INPUT);

    pinMode(AL, OUTPUT);
    pinMode(BL, OUTPUT);
    pinMode(CL, OUTPUT);
    digitalWrite(AL, 0);
    digitalWrite(BL, 0);
    digitalWrite(CL, 0);

}

BLDC::~BLDC()
{

}

void BLDC::initPWM()
{
    // begin with 500 pwm frequency
    Palatis::SoftPWM.begin(500);

    // print interrupt load for diagnostic purposes
    Palatis::SoftPWM.printInterruptLoad();

    Palatis::SoftPWM.allOff();
}

void BLDC::ReadHalls()
{
    RawHallData[HALL1_INDEX] = digitalRead(HALL1);
    RawHallData[HALL2_INDEX] = digitalRead(HALL2);
    RawHallData[HALL3_INDEX] = digitalRead(HALL3);
    newCommunationState = (commumationStates)((RawHallData[HALL1_INDEX] << HALL1_SHIFT) | (RawHallData[HALL2_INDEX] << HALL2_SHIFT) | RawHallData[HALL3_INDEX]);
}


void BLDC::Control()
{

    ReadHalls();
    if(started || directionState == CHANGING)
    {
        CalculateCommutationState();
    }

    ProcessMessages();
}

void BLDC::InputTest()
{
    controlPWM = 50;
    ReadHalls();
    currentCommunationState = newCommunationState;
    SetStateIO();

    delay(100);
}

void BLDC::FullCycleTest()
{
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

}


void BLDC::CalculateCommutationState()
{
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

}

void BLDC::SetStateIO()
{

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
}

int *BLDC::getRawHallData()
{
    return (int *)RawHallData;
}


void BLDC::startMotor(bool start)
{
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

}

int BLDC::findIndex(commumationStates state)
{
    for(int i = 0;i < COMMUTATION_STATES; i++)
    {
        if(startSeqeunce[i] == state)
            return i;
    }
}



void BLDC::CalculatePWM()
{
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

}

void BLDC::ChangeDirection(bool _forward)
{
    if(_forward == forward)
        return;

    startMotor(false);
    directionState = CHANGING;
    forward = _forward;

}

