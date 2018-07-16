# arm-bldc
Porting code from Arduino to ST Nucleo-64(STM32F302)
mbed os 5.8.3
use with the OpenBLDC shield v3

export PATH=$PATH:/opt/gcc-arm-none-eabi/bin/ 
mbed target NUCLEO_F303RE && mbed toolchain GCC_ARM

mbed compile --profile mbed-os/tools/profiles/debug.json

arduino     function    F303
D11         HIN_1       PA7
D10         HIN_2       PB6
D09         HIN_3       PC7
D07         LIN_1       PA8
D06         LIN_2       PB10
D05         LIN_3       PB4

D04         HALL_1      PB5
D03         HALL_2      PB3
D02         HALL_3      PA10

A00         Low Voltage PA0
A01         Current     PA1
