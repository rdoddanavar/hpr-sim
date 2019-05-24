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
#     hpr-sim/src/preproc/load_input.py
# Created:
#     2019-05-17
# Type:
#     Python3 script
# Description:
#     Parses input file(s) (*.yaml) to populate input classes
# Input:
#     *.yaml
# Output:
#     None
# Dependencies:
#     hpr-sim/
#         src/
#         preproc/
#             input.cpp
#             input.h
#------------------------------------------------------------#

# Installed modules
import sys  # System utilities
import pdb  # Python debugger
import yaml # YAML parser

# Path modifications
sys.path.insert(0, "../bin/")

# Program modules
import input # Input classes 

#----------------------------#

# YAML parse
inputDir   = "../input/"
inputName  = "input.yaml"
configName = "param_config.yaml"

filePath  = inputDir + inputName
stream    = open(filePath,'r')
inputDict = yaml.safe_load(stream)

filePath   = inputDir + configName
stream     = open(filePath,'r')
configDict = yaml.safe_load(stream)  

# test
class Object:
    pass

inputObj = Object()

# Param assignment & validation
for group in inputDict.keys():
    for param in inputDict[group].keys():
        for field in inputDict[group][param].keys():

            value = inputDict[group][param][field]

            if (isinstance(value,float) or isinstance(value, int)):
                paramObj = input.Param(float(value))

            elif (isinstance(value,str)):
                paramObj = input.Name(value)

            else:
                print("Input error: invalid type\n")

            pdb.set_trace()

# None of this is correct buddy
# Remove constructor, only use default 
# Nah accept both cases; if not a dict, or if all fields provided
# isinstance(inputObj,dist)

pdb.set_trace()