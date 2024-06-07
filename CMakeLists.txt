# PROJECT SETTINGS

cmake_minimum_required(VERSION 3.19) # Minumum required to use CMakePresets.json

project(hpr-sim DESCRIPTION "High Power Rocktry - Flight Simulation"
                LANGUAGES CXX
                VERSION 0.0.0
                )

#-----------------------------------------------------------------------------#

# BUILD SETTINGS

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# GNU GSL - minimize build
set(NO_AMPL_BINDINGS  true)
set(GSL_DISABLE_TESTS true)

set(BUILDLIBS "interpolation"
              "ode-initval2"
              )

#-----------------------------------------------------------------------------#

# SOURCE SUBDIRECTORIES

add_subdirectory(lib/pybind11) # Provides CMake helper functions
add_subdirectory(lib/gsl)      # Provides math utilities
add_subdirectory(src)          # CPP modules via pybind11

# lib/eigen is header-only, does not need to be a compilation target
