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
import pathlib

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

    # Validate input groups
    groupValid   = configDict.keys()
    groupInvalid = set(inputDict.keys()) - set(groupValid)

    for key in groupInvalid:
        inputDict.pop(key)

    groupMissing = set(groupValid) - set(inputDict.keys())

    if groupMissing:
        raise ValueError("Missing input groups(s)", groupMissing)

    # Validate input parameters in each input group

    for group in groupValid:
        
        paramValid   = configDict[group].keys()
        paramInvalid = set(inputDict[group].keys()) - set(paramValid)
        
        for key in paramInvalid:
            inputDict[group].pop(key)

        paramMissing = set(paramValid) - set(inputDict[group].keys())

        if paramMissing:
            raise ValueError("Missing input parameter(s)", group, paramMissing)

    # Input parameter value processing & validation

    for group in inputDict.keys():

        for param in inputDict[group].keys():
            
            # Get input parameter value & properties

            temp = inputDict[group][param]

            if type(temp) is dict:

                props = temp.keys()

                if "value" in props:
                    value = inputDict[group][param]["value"]
                else:
                    raise ValueError("Nominal parameter value missing", param)

            else:
                
                # Implied parameter value
                inputDict[group][param]          = {}
                inputDict[group][param]["value"] = temp

            # Process input parameter by type
            paramType = configDict[group][param]["type"]

            if (paramType == "int") or (paramType == "float"):
                process_number(inputDict, configDict, group, param)

            elif paramType == "str":
                process_string(inputDict, configDict, group, param)

            else:
                raise ValueError("Invalid input parameter type; see config files", param)

#------------------------------------------------------------------------------#

def process_number(inputDict, configDict, group, param):
    
    value     = inputDict[group][param]["value"]
    paramType = configDict[group][param]["type"]

    # Check if numeric

    try:
        value = float(value)
    except:
        raise ValueError("Input parameter is not numeric", param, value)

    # Check if integer

    if (paramType == "int") and (not value.is_integer()):
        raise ValueError("Input parameter is not integer", param, value)

    # Convert units if specified by user
    props = inputDict[group][param].keys()

    if "unit" in props:
        
        quantity = configDict[group][param]["quantity"]
        unit     = inputDict[group][param]["unit"]

        if quantity:
            value = util_unit.convert(value, quantity, unit)

    # Check parameter value bounds
    paramMin = configDict[group][param]["min"]
    paramMax = configDict[group][param]["max"]

    check_bounds(param, value, paramMin, paramMax)

    # Finalize parameter value

    if paramType == "int":
        value = int(value)

    inputDict[group][param]["value"] = value

#------------------------------------------------------------------------------#

def process_string(inputDict, configDict, group, param):

    value = inputDict[group][param]["value"]
    
    # Validate path

    if configDict[group][param]['isPath']:
        check_path(value)

    # TODO: validate string choices

#------------------------------------------------------------------------------#

def check_bounds(param, value, paramMin, paramMax):

    '''
    Checks value against lower & upper bounds.

    Inputs(s): parameter (str), value (float), paramMin (float), paramMax (float)
    '''

    if (value < paramMin) or (value > paramMax):
        raise ValueError("Input parameter violates bounds", param, value)

#------------------------------------------------------------------------------#

def check_path(value):

    '''
    Checks for file path existence.

    Input(s): value (str)
    '''
    
    if not os.path.exists(value):
        raise FileNotFoundError(value)

#------------------------------------------------------------------------------#

if __name__ == "__main__":

    # CLI argument
    pass