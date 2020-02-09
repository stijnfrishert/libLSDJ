cmake_minimum_required(VERSION 3.0.0)

if (APPLE)
  # Add global definitions and include directories
  add_definitions(-Wall -Werror -Wconversion -Wno-unused-variable)
endif (APPLE)

set(HEADERS
	channel.h
	command.h
	compression.h
	error.h
	project.h
	sav.h
	song.h
	version.h
	vio.h
	)

set(SOURCES
	compression.c
	error.c
	project.c
	sav.c
	vio.c
	)

# Create the library target
add_library(liblsdj STATIC ${HEADERS} ${SOURCES})
set_target_properties(liblsdj PROPERTIES OUTPUT_NAME lsdj)
source_group(\\ FILES ${HEADERS} ${SOURCES})
target_include_directories(liblsdj INTERFACE ".")

install(TARGETS liblsdj DESTINATION lib)
install(FILES ${HEADERS} DESTINATION include/lsdj)

add_subdirectory(test)