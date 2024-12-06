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
import util_misc

# Module variables
configInput = None

#------------------------------------------------------------------------------#

def config() -> None:

    '''
    Parses YAML config file, creates global dict for input parameter validation
    '''

    global configInput # Necessary for reassignment

    if configInput is None:

        if util_misc.is_bundled():
            configPath = pathlib.Path("_internal") / "config_input.yml"
        else:
            configPath = pathlib.Path(__file__).parent / "config_input.yml"

        configInput = util_yaml.load(configPath)
        util_yaml.process(configInput)

#------------------------------------------------------------------------------#

def process(inputParams: dict) -> None:

    '''
    Populates input parameters via YAML input; converts and validates parameters.

    :param inputParams: Input parameters and their values
    '''

    # Validate input groups
    groupValid   = configInput.keys()
    groupInvalid = set(inputParams.keys()) - set(groupValid)

    for key in groupInvalid:
        inputParams.pop(key)

    groupMissing = set(groupValid) - set(inputParams.keys())

    if groupMissing:
        raise ValueError("Missing input groups(s)", groupMissing)

    # Validate input parameters in each input group

    for group in groupValid:

        paramValid   = configInput[group].keys()
        paramInvalid = set(inputParams[group].keys()) - set(paramValid)

        for key in paramInvalid:
            inputParams[group].pop(key)

        paramMissing = set(paramValid) - set(inputParams[group].keys())

        if paramMissing:
            raise ValueError("Missing input parameter(s)", group, paramMissing)

    # Input parameter value processing & validation

    for group in inputParams.keys():

        for param in inputParams[group].keys():

            # Get input parameter value & properties

            temp = inputParams[group][param]

            if type(temp) is dict:

                props = temp.keys()

                if "value" not in props:
                    raise ValueError("Nominal parameter value missing", param)

            else:

                # Implied parameter value
                inputParams[group][param]          = {}
                inputParams[group][param]["value"] = temp

            # Process input parameter by type
            paramType = configInput[group][param]["type"]

            if (paramType == "int") or (paramType == "float"):
                process_number(inputParams, group, param)

            elif paramType == "str":
                process_string(inputParams, group, param)

            elif paramType == "bool":
                process_bool(inputParams, group, param)

            else:
                raise ValueError("Invalid input parameter type; see config files", param)

#------------------------------------------------------------------------------#

def process_number(inputParams: dict, group: str, param: str) -> None:

    value     = inputParams[group][param]["value"]
    paramType = configInput[group][param]["type"]

    # Check if numeric

    try:
        value = float(value)
    except:
        raise ValueError("Input parameter is not numeric", param, value)

    # Check if integer

    if (paramType == "int") and (not value.is_integer()):
        raise ValueError("Input parameter is not integer", param, value)

    # Convert units if specified by user
    props = inputParams[group][param].keys()

    if "unit" in props:

        quantity = configInput[group][param]["quantity"]
        unit     = inputParams[group][param]["unit"]

        if quantity:
            value = util_unit.convert(value, quantity, unit)

    # Check parameter value bounds
    paramMin = configInput[group][param]["min"]
    paramMax = configInput[group][param]["max"]

    if (value < paramMin) or (value > paramMax):
        raise ValueError("Input parameter violates bounds", param, value)

    # Check if parameter can be randomly sampled 
    paramRand = configInput[group][param]["isRand"]

    if (not paramRand) and ("dist" in props):
        raise ValueError("Input parameter does not allow a random distribution", param)

    # Finalize parameter value

    if paramType == "int":
        value = int(value)

    inputParams[group][param]["value"] = value

#------------------------------------------------------------------------------#

def process_string(inputParams: dict, group: str, param: str) -> None:

    value = inputParams[group][param]["value"]

    # Validate path

    if configInput[group][param]["isPath"]:
        if not os.path.exists(value):
            raise FileNotFoundError(value)

    # Validate string choice

    if "valid" in configInput[group][param].keys():
        if value not in configInput[group][param]["valid"]:
            raise ValueError("Invalid choice for input parameter", param, value)
        
    # Ensure that string parameter doesn't have random distribution

    if "dist" in inputParams[group][param].keys():
        raise ValueError("Input parameter does not allow a random distribution", param)

#------------------------------------------------------------------------------#

def process_bool(inputParams: dict, group: str, param: str) -> None:

    value = inputParams[group][param]["value"]

    if (value is True) or (value is False):
        pass # Valid, do nothing
    else:
        raise ValueError("Input parameter must be one of: True, true, False, false", param)

#------------------------------------------------------------------------------#

if __name__ == "__main__":
    pass
else:
    config()
