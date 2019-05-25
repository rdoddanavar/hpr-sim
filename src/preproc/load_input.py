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
#     hpr-sim/src/preproc/input.cpp
#                        /input.h
#------------------------------------------------------------#

# Installed modules
import sys  # System utilities
import pdb  # Python debugger
import yaml # YAML parser

# Path modifications
sys.path.insert(0, "../../bin/")

# Program modules
import input # Input classes 

def load_yaml(filePath):

    stream   = open(filePath, 'r')
    yamlDict = yaml.safe_load(stream)

    return yamlDict

def load_input(inputPath):

    # YAML parse
    configPath = "./param_config.yaml"
    configDict = load_yaml(configPath)
    inputDict  = load_yaml(inputPath)

    # Instantiate input object
    inp = input.Input()

    # Param config

    for group in configDict.keys():
        for param in configDict[group].keys():
                for field in configDict[group][param].keys():

                    value = configDict[group][param][field]
                    setattr(getattr(getattr(inp,group),param),field,value)

    # Param assignment

    for group in inputDict.keys():

        for param in inputDict[group].keys():

            # Multiple fields specified by user
            if (isinstance(inputDict[group][param], dict)):

                for field in inputDict[group][param].keys():

                    value = inputDict[group][param][field]
                    setattr(getattr(getattr(inp,group),param),field,value)

            # Only "value" is specified by user
            else: 

                value = inputDict[group][param]
                getattr(getattr(inp,group),param).value = value

    # Param conversion


    # Param validation 

    for group in inputDict.keys():
        for param in inputDict[group].keys():
                
                value = getattr(getattr(inp,group),param).value

                if (isinstance(value, float)):
                    cond = getattr(getattr(inp,group),param).checkValue()

                elif (isinstance(value, str)):
                    cond = getattr(getattr(inp,group),param).checkPath()

                print(param + ": ", cond)

    pdb.set_trace()

if __name__ == "__main__":
    load_input(sys.argv[1])