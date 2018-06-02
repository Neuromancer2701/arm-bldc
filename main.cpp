// This file has been prepared for Doxygen automatic documentation generation.
//Seth M King
#include "bldc.h"


int main()
{
    BLDC motor;
    printf("\n\n*** motor serial test ***\n");

    while(1)
    {
      motor.Control();
    };

}
