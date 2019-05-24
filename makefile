#------------------------------------------------------------#
# High Power Rocketry - Flight Simulation
# GNU General Public License v3.0
# Roshan Doddanavar
# https://rdoddanavar.github.io
#------------------------------------------------------------#
#     ___       ___       ___       ___       ___       ___
#    /\__\     /\  \     /\  \     /\  \     /\  \     /\__\  
#   /:/__/_   /::\  \   /::\  \   /::\  \   _\:\  \   /::L_L_
#  /::\/\__\ /::\:\__\ /::\:\__\ /\:\:\__\ /\/::\__\ /:/L:\__\
#  \/\::/  / \/\::/  / \;:::/  / \:\:\/__/ \::/\/__/ \/_/:/  /
#    /:/  /     \/__/   |:\/__/   \::/  /   \:\__\     /:/  /
#    \/__/               \|__|     \/__/     \/__/     \/__/  
# 
#------------------------------------------------------------#
# Path:
#     hpr-sim/makefile
# Created:
#     2019-04-21
# Type:
#     Makefile script
# Description:
#     Build automation script via GNU Make
# Input:
#     None
# Output:
#     None
# Dependencies:
#     None
#------------------------------------------------------------#

# Settings & flags
CXX=g++
CXXFLAGS=-O3 -Wall -shared -std=c++11 -fPIC
PYINCLUDE=`python3 -m pybind11 --includes`
PYSUFFIX=`python3-config --extension-suffix`
SRCDIR=./src/
BINDIR=./bin/

SRC=example.cpp

# Build process
all: input

input: 
	$(CXX) $(CXXFLAGS) $(PYINCLUDE) $(SRCDIR)input.cpp -o $(BINDIR)$@$(PYSUFFIX)

clean:
	cd ./bin/ && rm *