cmake_minimum_required(VERSION 3.0.0)

# Create the project
project(lsdj)

# Add global definitions and include directories
add_definitions(-Wall -Werror -Wconversion)

# Create the library target
add_library(liblsdj STATIC chain.h chain.c command.h command.c compression.h compression.c error.h error.c instrument.h instrument.c phrase.h phrase.c project.h project.c row.h row.c sav.h sav.c song.h song.c table.h table.c vio.h vio.c)
set_target_properties(liblsdj PROPERTIES OUTPUT_NAME lsdj)

# Create the executable target
add_executable(lsdj main.c)
target_link_libraries(lsdj liblsdj)

install(TARGETS liblsdj DESTINATION lib)
install(FILES compression.h error.h instrument.h project.h sav.h song.h DESTINATION include/lsdj)
