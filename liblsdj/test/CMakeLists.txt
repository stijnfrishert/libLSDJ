cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

add_executable(test
	error.cpp
	file.cpp
	file.hpp
	main.cpp
	project.cpp
	sav.cpp
	song_buffer.cpp
	vio.cpp
	)

target_compile_definitions(test PRIVATE RESOURCES_FOLDER="${CMAKE_SOURCE_DIR}/resources/")

target_link_libraries(test
	PRIVATE
	Catch2::Catch2
	liblsdj
	)

# include(CTest)
# include(Catch)
# catch_discover_tests(test)