'''
High Power Rocketry - Flight Simulation
GNU General Public License v3.0
Copyright (C) 2019 Roshan Doddanavar
https://rdoddanavar.github.io

██╗  ██╗██████╗ ██████╗       ███████╗██╗███╗   ███╗
██║  ██║██╔══██╗██╔══██╗      ██╔════╝██║████╗ ████║
███████║██████╔╝██████╔╝█████╗███████╗██║██╔████╔██║
██╔══██║██╔═══╝ ██╔══██╗╚════╝╚════██║██║██║╚██╔╝██║
██║  ██║██║     ██║  ██║      ███████║██║██║ ╚═╝ ██║
╚═╝  ╚═╝╚═╝     ╚═╝  ╚═╝      ╚══════╝╚═╝╚═╝     ╚═╝

Path:
    hpr-sim/src/preproc/preproc_input.py
Created:
    2019-05-17
Type:
    Python3 module
Description:
    Parses YAML input files to populate input classes.
Functions:
    load
Classes:
    None
Dependencies:
    hpr-sim/src/preproc/input.cpp
                       /input.h
                /util/util_yaml.py
                     /util_unit.py
'''

# System modules
import sys  # System utilities
import pdb  # Python debugger
import yaml # YAML parser

# Path modifications
sys.path.insert(0, "../../bin/")
sys.path.insert(0, "../util/")

# Program modules
import util_yaml
import util_unit

# Pybind11 modules
import input # Input classes 

def preproc_input(inputPath):

    '''
    Populates input parameters via YAML input; converts and validates parameters.
    '''

    # YAML parse
    configPath = "./config_param.yaml"
    configDict = util_yaml.load(configPath)
    inputDict  = util_yaml.load(inputPath)

    # Instantiate input object
    inp = input.Input()

    # Param config

    for group in configDict.keys():
        for param in configDict[group].keys():
                for field in configDict[group][param].keys():

                    value = configDict[group][param][field]
                    setattr(getattr(getattr(inp, group), param), field, value)

    # Param assignment

    for group in inputDict.keys():

        for param in inputDict[group].keys():

            # Multiple fields specified by user
            if (isinstance(inputDict[group][param], dict)):

                for field in inputDict[group][param].keys():

                    value = inputDict[group][param][field]
                    setattr(getattr(getattr(inp, group), param), field, value)

            # Only "value" is specified by user
            else: 

                value = inputDict[group][param]
                getattr(getattr(inp, group), param).value = value

    # Param conversion & validation 

    unitDict = util_unit.config()

    for group in inputDict.keys():
        for param in inputDict[group].keys():
                
            obj = getattr(getattr(inp, group), param)

            if (isinstance(obj, input.Param)):

                factor     = util_unit.convert(unitDict, obj.quantity, obj.unit)
                obj.value *= factor

                print("factor: ", factor)

                cond = obj.checkValue()

            elif (isinstance(obj, input.Name)):
                cond = obj.checkPath()

            
            print(param + ": ", cond)

    pdb.set_trace()

if __name__ == "__main__":

    # CLI argument
    load_input(sys.argv[1])