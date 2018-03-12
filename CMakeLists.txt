cmake_minimum_required(VERSION 3.0)
project(NameTag)

# Build with
# mkdir build_dir
# cd build_dir
# cmake -DCMAKE_TOOLCHAIN_FILE=<NameTag-source-dir>/cmake-avr/generic-gcc-avr.cmake -DAVR_MCU=atmega328 -DMCU_SPEED=8000000UL <NameTag-source-dir>

# if your avrdude cant find its config file (e.g. the avrdude from arduino) append this to the cmake call
# -DAVR_UPLOADTOOL_OPTIONS='-C/opt/arduino-1.6.7/hardware/tools/avr/etc/avrdude.conf' ..

add_avr_library(${PROJECT_NAME}
   matrixdisplay.cpp
   nametag.cpp
   statemachine.h
   timing.c
   names.h
)

add_avr_executable(main
   main.cpp
   nametag_sm.cpp
)

avr_target_link_libraries(main ${PROJECT_NAME})
