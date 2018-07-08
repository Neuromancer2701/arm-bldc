# arm-bldc
Porting code from Arduino to ST Nucleo-64(STM32F302)
mbed os 5.8.3
use with the OpenBLDC shield v3

export PATH=$PATH:/opt/gcc-arm-none-eabi/bin/ 
mbed target NUCLEO_F303RE && mbed toolchain GCC_ARM

mbed compile --profile mbed-os/tools/profiles/debug.json


