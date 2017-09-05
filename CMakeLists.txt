cmake_minimum_required(VERSION 3.0.0)

# Create the project
project(lsdj)

# Add global definitions and include directories
add_definitions(-Wall -Werror -Wconversion)

# Create the target
add_library(lsdj STATIC error.h error.c project.h project.c sav.h sav.c)

install(TARGETS lsdj DESTINATION lib)
install(FILES error.h project.h sav.h DESTINATION include/lsdj)
