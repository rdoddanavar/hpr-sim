# Project settings

cmake_minimum_required(VERSION 3.10.2) # Ubuntu 18.04 LTS default

project(hpr-sim DESCRIPTION "High Power Rocktry - Flight Simulation"
                LANGUAGES CXX)

# Build settings

set(CMAKE_POSITION_INDEPENDENT_CODE ON) # for shared libs; delete in future if unnecessary

# Source subdirectories

add_subdirectory(lib/pybind11)
#add_subdirectory(lib/eigen)
add_subdirectory(src)
