cmake_minimum_required(VERSION 3.5)
project(BLDC_ros_controller)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES
    main.cpp
    bldc/bldc.cpp
    bldc/bldc.h
    comms/comms.cpp
    comms/comms.cpp
        )

include_directories(bldc comms mbed-os/drivers mbed-os)

add_executable(arm-bldc ${SOURCE_FILES})