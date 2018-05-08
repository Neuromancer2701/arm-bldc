cmake_minimum_required(VERSION 3.5)
project(arm-bldc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
add_definitions(-DTARGET_NUCLEO_F302)

#set(SOURCE_FILES
#    main.cpp
#    bldc/bldc.cpp
#    bldc/bldc.h
#    comms/comms.cpp
#    comms/comms.h
#    mbed-os/mbed.h
#    mbed-os/drivers/Serial.h
#        )

file(GLOB SOURCE_FILES "./*.cpp")

include_directories(bldc
        comms
        mbed-os/drivers
        mbed-os/events
        mbed-os/platform
        mbed-os/rtos
        mbed-os)

add_executable(arm-bldc ${SOURCE_FILES})