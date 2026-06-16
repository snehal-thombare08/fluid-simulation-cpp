cmake_minimum_required(VERSION 3.16)
project(FluidSim)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML COMPONENTS Graphics Window System CONFIG REQUIRED)

add_executable(FluidSim src/main.cpp)

target_link_libraries(FluidSim PRIVATE SFML::Graphics SFML::Window SFML::System)