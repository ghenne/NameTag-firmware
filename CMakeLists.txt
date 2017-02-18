cmake_minimum_required(VERSION 3.0)
project(NameTag)

set(AVR_MCU atmega328p)
set(MCU_SPEED 1000000UL)
set(AVR_PROGRAMMER usbasp)

add_avr_library(${PROJECT_NAME}
   matrixdisplay.cpp
   nametag.cpp
   statemachine.h
   timing.c
   avr.h
)

if(1)

add_avr_executable(main
   main.cpp
   nametag_sm.cpp
)
target_link_libraries(main${MCU_TYPE_FOR_FILENAME}.elf ${PROJECT_NAME}${MCU_TYPE_FOR_FILENAME})

else()

add_avr_executable(maintest
   maintest.cpp
)
target_link_libraries(maintest${MCU_TYPE_FOR_FILENAME}.elf ${PROJECT_NAME}${MCU_TYPE_FOR_FILENAME})

endif()
