cmake_minimum_required(VERSION 3.0.0)

if (APPLE)
  # Add global definitions and include directories
  add_definitions(-Wall -Werror -Wconversion -Wno-unused-variable)
endif (APPLE)

set(HEADERS
	bytes.h
	chain.h
	channel.h
	command.h
	compression.h
	error.h
	instrument.h
	panning.h
	phrase.h
	project.h
	sav.h
	song.h
	synth.h
	table.h
	version.h
	wave.h
	vio.h
	)

set(SOURCES
	bytes.c
	compression.c
	chain.c
	error.c
	groove.c
	instrument.c
	phrase.c
	project.c
	sav.c
	song.c
	synth.c
	table.c
	vio.c
	wave.c
	)

# Create the library target
add_library(liblsdj STATIC ${HEADERS} ${SOURCES})
set_target_properties(liblsdj PROPERTIES OUTPUT_NAME lsdj)
source_group(\\ FILES ${HEADERS} ${SOURCES})
target_include_directories(liblsdj INTERFACE ".")

install(TARGETS liblsdj DESTINATION lib)
install(FILES ${HEADERS} DESTINATION include/lsdj)

add_subdirectory(test)