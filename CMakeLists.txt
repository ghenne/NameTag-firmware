cmake_minimum_required(VERSION 3.0)
project(NameTag)

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
target_link_libraries(main ${PROJECT_NAME})
