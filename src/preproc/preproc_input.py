'''
High Power Rocketry - Flight Simulation
MIT License
Copyright (C) 2019 Roshan Doddanavar
https://rdoddanavar.github.io

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
    <none>
'''

# System modules
import os

# Project modules
import util_yaml
import util_unit

#------------------------------------------------------------------------------#

def process(inputDict, configDict):

    '''
    Populates input parameters via YAML input; converts and validates parameters.

    Input(s): inputDict (dict), configDict (dict) \n
    Output(s): <none>
    ''' 

    groupValid = configDict.keys()

    # Param conversion & validation 
    util_unit.config()

    for group in inputDict.keys():

        if group in groupValid:

            for param in inputDict[group].keys():
                
                temp = inputDict[group][param]

                if type(temp) is dict:
                    props = temp.keys()

                    if "value" in props:
                        value = inputDict[group][param]["value"]

                else:
                    props = []
                    value = temp

                # Convert units if specified by user
                if "unit" in props:

                    value    = inputDict[group][param]["value"]
                    quantity = configDict[group][param]["quantity"]
                    unit     = inputDict[group][param]["unit"]

                    value = util_unit.convert(value, quantity, unit)

                if "isPath" in configDict[group][param].keys():
                    check_path(value)
                else:

                    paramMin = configDict[group][param]["min"]
                    paramMax = configDict[group][param]["max"]

                    check_value(param, value, paramMin, paramMax)

def check_value(param, value, paramMin, paramMax):

    if (value >= paramMin) and (value <= paramMax):
        return True
    else:
        raise ValueError("Input parameter violates bounds", param, value)

def check_path(value):
    
    if os.path.exists(value):
        return True
    else:
        raise FileNotFoundError(value)

if __name__ == "__main__":

    # CLI argument
    pass