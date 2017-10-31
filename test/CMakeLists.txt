cmake_minimum_required(VERSION 3.0.0)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS filesystem program_options)

# Create the executable target
add_executable(lsdj-test main.cpp)
source_group(\\ FILES main.cpp)

target_compile_features(lsdj-test PUBLIC cxx_std_14)
target_include_directories(lsdj-test PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(lsdj-test liblsdj ${Boost_LIBRARIES})
