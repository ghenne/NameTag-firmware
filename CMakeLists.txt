cmake_minimum_required(VERSION 3.0)
project(Namenschild)

set(AVR_MCU atmega8)
set(MCU_SPEED 4000000UL)
set(AVR_PROGRAMMER usbasp)

add_avr_executable(${PROJECT_NAME} main.c)
