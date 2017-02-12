cmake_minimum_required(VERSION 3.0)
project(Namenschild)

set(AVR_MCU atmega328p)
set(MCU_SPEED 1000000UL)
set(AVR_PROGRAMMER usbasp)

add_avr_executable(${PROJECT_NAME}
   main.cpp

   matrixdisplay.cpp
   nametag.cpp
   timing.c
   avr.h
)
