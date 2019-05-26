# High Power Rocketry - Flight Simulation
# GNU General Public License v3.0
# Copyright (C) 2019 Roshan Doddanavar
# https://rdoddanavar.github.io
# 
# ██╗  ██╗██████╗ ██████╗       ███████╗██╗███╗   ███╗
# ██║  ██║██╔══██╗██╔══██╗      ██╔════╝██║████╗ ████║
# ███████║██████╔╝██████╔╝█████╗███████╗██║██╔████╔██║
# ██╔══██║██╔═══╝ ██╔══██╗╚════╝╚════██║██║██║╚██╔╝██║
# ██║  ██║██║     ██║  ██║      ███████║██║██║ ╚═╝ ██║
# ╚═╝  ╚═╝╚═╝     ╚═╝  ╚═╝      ╚══════╝╚═╝╚═╝     ╚═╝
# 
# Path:
#     hpr-sim/makefile
# Created:
#     2019-04-21
# Type:
#     Makefile script
# Description:
#     Build automation script via GNU Make.
# Functions:
#     None
# Classes:
#     None
# Dependencies:
#     None

# Settings & flags
CXX=g++
CXXFLAGS=-O3 -Wall -shared -std=c++11 -fPIC
PYINCLUDE=`python3 -m pybind11 --includes`
PYSUFFIX=`python3-config --extension-suffix`
SRCDIR=./src/preproc/
BINDIR=./bin/

SRC=example.cpp

# Build process
all: input

input: input.cpp input.h
	$(CXX) $(CXXFLAGS) $(PYINCLUDE) $(SRCDIR)$< -o $(BINDIR)$@$(PYSUFFIX)

input.cpp:

input.h:

clean:
	cd ./bin/ && rm *