// This file has been prepared for Doxygen automatic documentation generation.
//Seth M King

#ifndef __BLDC_H__
#define __BLDC_H__

#pragma GCC diagnostic ignored "-Wregister"


#include <mbed.h>
#include <memory>
#include <map>
#include "comms.h"

using std::unique_ptr;
using std::map;


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
    A_HIGH = D11,    //! Port pin connected to phase A, high side enable switch. Arduino Pin 11
    A_LOW = D7,      //! Port pin connected to phase A, low side enable switch.  Arduino Pin 7

    B_HIGH = D10,	 //! Port pin connected to phase B, high side enable switch. Arduino Pin 10
    B_LOW = D6,	     //! Port pin connected to phase B, low side enable switch. Arduino Pin 6

    C_HIGH = D9,	 //! Port pin connected to phase C, high side enable switch. Arduino Pin 9
    C_LOW = D5,      //! Port pin connected to phase C, low side enable switch.	Arduino Pin 5

    A_ON  = 0x04,  //100
    B_ON  = 0x02,  //010
    C_ON  = 0x01,  //001
    PWM_A = 0,
    PWM_B = 1,
    PWM_C = 2

};

enum HALL_PINS
{
    HALL1 = D4,	 //! Port pin connected to phase A, high side enable switch. Arduino Pin 4
    HALL1_SHIFT = 2,
    HALL1_INDEX = 0,

    HALL2 = D3, //! Port pin connected to phase B, low side enable switch.  Arduino Pin 3
    HALL2_SHIFT = 1,
    HALL2_INDEX = 1,

    HALL3 = D2,	 //! Port pin connected to phase C, high side enable switch. Arduino Pin 2
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

	void FullCycleTest();
	void InputTest();
    void Reverse(){data.forward = false;}
    void Forward(){data.forward = true;}
    void ReadHalls();


    enum constants
    {
        NUMBER_HALLS = 3,
        PWM_PERIOD_MS = 2,
        COMMUTATION_STATES = 6,
        DUTY_STOP = 0,
        AL_HIGH_PORTB = 0x10,
        BL_HIGH_PORTB = 0x01,
        CL_HIGH_PORTB = 0x20,
        CYCLES_PER_REV = 60,
        RADIUS = 165,
		SAMPLE_WINDOW_MS = 100,
		FET_IO = 3   // high side and low side each have 3
    };

private:

    commumationStates currentCommunationState;
    commumationStates newCommunationState;
    int cycleCounter;

	DataTransport data;
    Comms communication;
    int deltaT;
    int directionWindow;

	double error;
	double previousError;

	array<unique_ptr<PwmOut>, FET_IO> HighSide;
    array<PinName, FET_IO> HighSide_pins = {D11 /*A_HIGH*/, D10/*B_HIGH*/, D9/*C_HIGH*/};

    BusOut LowSide;
    BusIn  HallIO;

    map<int, pair<int, int> >  commutationMap;
    array<int, COMMUTATION_STATES> HighSideStates = {A_HIGH, B_HIGH, B_HIGH, C_HIGH, C_HIGH, A_HIGH};
    array<int, COMMUTATION_STATES> LowSideStates  = {C_LOW,  C_LOW,  A_LOW,  A_LOW,  B_LOW,  B_LOW};


    map<commumationStates, commumationStates >  forward2Reverse;
    array<commumationStates, COMMUTATION_STATES>  forwardSequence = {State1, State2, State3, State4, State5, State6};
    array<commumationStates, COMMUTATION_STATES>  reverseSequence = {State4, State5, State6, State1, State2, State3};

    void startMotor(bool start);
	void ChangeDirection(bool forward);
    void nextState();
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
	Timer speedTimer;

	void delta_T()
    {
        speedTimer.stop();
        deltaT = speedTimer.read_ms();
        speedTimer.reset();
        speedTimer.start();
	}
};



#endif 
