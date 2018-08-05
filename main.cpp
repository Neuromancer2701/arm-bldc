// This file has been prepared for Doxygen automatic documentation generation.
//Seth M King
#include "bldc.h"


int main()
{
    BLDC motor;

    motor.initPWM();

    while(1)
    {
      motor.FullCycleTest();
    };

}
