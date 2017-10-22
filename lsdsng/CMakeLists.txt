cmake_minimum_required(VERSION 3.0.0)

project(lsdsng CXX)

find_package(Boost REQUIRED COMPONENTS filesystem)

# Create the executable target
add_executable(lsdsng main.cpp)

target_include_directories(lsdsng PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(lsdsng liblsdj ${Boost_LIBRARIES})

install(TARGETS lsdsng DESTINATION bin)

message(STATUS ${Boost_INCLUDE_DIRS})
