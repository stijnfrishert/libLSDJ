cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

add_executable(test
	main.cpp
	error.cpp
	project.cpp
	sav.cpp
	song_buffer.cpp
	vio.cpp
	)

target_link_libraries(test
	PRIVATE
	Catch2::Catch2
	liblsdj
	)

# include(CTest)
# include(Catch)
# catch_discover_tests(test)