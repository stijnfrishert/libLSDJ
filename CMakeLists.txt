cmake_minimum_required(VERSION 3.0.0)

# Create the project
project(lsdj)

# Add global definitions and include directories
add_definitions(-Wall -Werror -Wconversion)

# Create the target
add_library(lsdj STATIC sav.h sav.c)

install(TARGETS lsdj DESTINATION lib)
install(FILES sav.h DESTINATION include/lsdj)