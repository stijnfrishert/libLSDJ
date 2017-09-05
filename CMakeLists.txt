cmake_minimum_required(VERSION 3.0.0)

# Create the project
project(lsdj)

# Add global definitions and include directories
add_definitions(-Wall -Werror -Wconversion)

# Create the library target
add_library(liblsdj STATIC error.h error.c project.h project.c sav.h sav.c)
set_target_properties(liblsdj PROPERTIES OUTPUT_NAME lsdj)

# Create the executable target
add_executable(lsdj main.c)
target_link_libraries(lsdj liblsdj)

install(TARGETS liblsdj DESTINATION lib)
install(FILES error.h project.h sav.h DESTINATION include/lsdj)
