# PROJECT SETTINGS

cmake_minimum_required(VERSION 3.10.2) # Ubuntu 18.04 LTS default

project(hpr-sim DESCRIPTION "High Power Rocktry - Flight Simulation"
                LANGUAGES CXX
                )

# BUILD SETTINGS

set(CMAKE_POSITION_INDEPENDENT_CODE ON) # For shared libs; delete in future if unnecessary

# SOURCE SUBDIRECTORIES

set(GSL_DISABLE_TESTS true) # Reduce build time

add_subdirectory(lib/pybind11) # Provides CMake helpers for src
add_subdirectory(lib/gsl)      # Tests disabled
add_subdirectory(src)          # CPP modules via pybind11

# lib/eigen is header-only, does not need to be a compilation target