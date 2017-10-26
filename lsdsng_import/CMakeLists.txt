cmake_minimum_required(VERSION 3.0.0)

find_package(Boost REQUIRED COMPONENTS filesystem program_options)

# Create the executable target
add_executable(lsdsng-import main.cpp)
source_group(\\ FILES main.cpp)

target_compile_features(lsdsng-import PUBLIC cxx_std_14)
target_include_directories(lsdsng-import PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(lsdsng-import liblsdj ${Boost_LIBRARIES})

install(TARGETS lsdsng-import DESTINATION bin)
