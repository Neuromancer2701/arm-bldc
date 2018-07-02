# arm-bldc
Porting code from Arduino to ST Nucleo-64(STM32F302)
mbed os 5.8.3
use with the OpenBLDC shield v3


export PATH=$PATH:/opt/gcc-arm-none-eabi/bin/
mbed target NUCLEO_F302R8
mbed toolchain GCC_ARM

mbed compile -t gcc_arm -m NUCLEO_F302R8 --profile mbed-os/tools/profiles/debug.json 

