// This file has been prepared for Doxygen automatic documentation generation.
//Seth M King

#ifndef __BLDC_H__
#define __BLDC_H__

#pragma GCC diagnostic ignored "-Wregister"


#include <mbed.h>
#include "comms.h"



enum commumationStates
{
    State1   = 0x04,
    State2   = 0x06,
    State3   = 0x02,
    State4   = 0x03,
    State5   = 0x01,
    State6   = 0x05
};

enum FET_PINS
{
    AH = 4,	 //! Port pin connected to phase A, high side enable switch. Arduino Pin 4
    AL = 12, //! Port pin connected to phase A, low side enable switch.  Arduino Pin 12

    BH = 9,	 //! Port pin connected to phase B, high side enable switch. Arduino Pin 9
    BL = 8,	 //! Port pin connected to phase B, low side enable switch. Arduino Pin 8

    CH = 5,	 //! Port pin connected to phase C, high side enable switch. Arduino Pin 5
    CL = 13, //! Port pin connected to phase C, low side enable switch.	Arduino Pin 13

    AH_INDEX = 0x00,
    BH_INDEX = 0x01,
    CH_INDEX = 0x02
};

enum HALL_PINS
{
    HALL1 = 3,	 //! Port pin connected to phase A, high side enable switch. Arduino Pin 2
    HALL1_SHIFT = 2,
    HALL1_INDEX = 0,

    HALL2 = 6, //! Port pin connected to phase A, low side enable switch.  Arduino Pin 1
    HALL2_SHIFT = 1,
    HALL2_INDEX = 1,
    HALL3 = 7,	 //! Port pin connected to phase B, high side enable switch. Arduino Pin 0
    HALL3_INDEX = 2,
};


class BLDC
{
public:
	BLDC();
	virtual ~BLDC();

	void Control();
	void CalculateCommutationState();
    void initPWM();

    commumationStates getCommunationState() { return currentCommunationState; }
    void setCurrentCommunationState(commumationStates CurrentCommunationState) {  currentCommunationState = CurrentCommunationState; }

    void setNewCommunationState(commumationStates NewCommunationState) { newCommunationState = NewCommunationState; }

    int* getRawHallData();
	void FullCycleTest();
	void InputTest();
    void Reverse(){data.forward = false;}
    void Forward(){data.forward = true;}
    void ReadHalls();


    enum constants
    {
        NUMBER_HALLS = 3,
        PWM_FREQUENCY = 100,
        COMMUTATION_STATES = 6,
        DUTY_STOP = 0,
        AL_HIGH_PORTB = 0x10,
        BL_HIGH_PORTB = 0x01,
        CL_HIGH_PORTB = 0x20,
        CYCLES_PER_REV = 60,
        RADIUS = 165,
		SAMPLE_WINDOW_MS = 100
    };

private:

    volatile int RawHallData[NUMBER_HALLS];
    commumationStates currentCommunationState;
    commumationStates newCommunationState;
    int cycleCounter;

	DataTransport data;
    Comms communication;
    long previousTime;
    long currentTime;
    long directionWindow;


	double error;
	double previousError;


    void startMotor(bool start);
	void ChangeDirection(bool forward);
    int  findIndex(commumationStates state);
	void CalculatePWM();
    void SetStateIO();



	enum velocityConstants
	{
		MAX_PWM = 85,
		MIN_PWM = 15,
        MAX_PWM_STEP = 4
	};

	enum changeDirection
	{
		REVERSE  = 0,
		FORWARD  = 1,
		CHANGING = 2
	};

	changeDirection directionState;


};



#endif 
