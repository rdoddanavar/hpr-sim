# Project settings

cmake_minimum_required(VERSION 3.10.2)

project(hpr-sim DESCRIPTION "High Power Rocktry - Flight Simulation"
                LANGUAGES CXX)

# Directory management 

set(LIBRARY_DIR "${CMAKE_SOURCE_DIR}/../lib")

# Build settings

# External libraries

add_subdirectory("${LIBRARY_DIR}/pybind11")
add_subdirectory("${LIBRARY_DIR}/eigen")

# Project subdirectories 