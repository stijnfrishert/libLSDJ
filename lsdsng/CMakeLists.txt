cmake_minimum_required(VERSION 3.0.0)

find_package(Boost REQUIRED COMPONENTS filesystem program_options)

# Create the executable target
add_executable(lsdsng main.cpp)

target_compile_features(lsdsng PUBLIC cxx_std_14)
target_include_directories(lsdsng PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(lsdsng liblsdj ${Boost_LIBRARIES})

install(TARGETS lsdsng DESTINATION bin)