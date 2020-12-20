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
    hpr-sim/src/util/util_unit.py
Created:
    2019-05-25
Type:
    Python3 module
Description:
    Provides unit conversion utilities.
Functions:
    config
    convert
Classes:
    None
Dependencies:
    hpr-sim/src/util/util_yaml
                    /config_unit.yaml
'''

# System modules
import sys  # System utilities
import pdb  # Python debugger
import yaml # YAML parser

# Project modules
import util_yaml

# Module variables
unitDict = None

def config():

    '''
    Parses YAML config file, creates global dict of unit conversion factors.

    Input(s): <none>
    Outputs(s): <none>
    '''

    global unitDict # Necessary for reassignment

    if (not unitDict):

        configPath = "../config/config_unit.yaml"
        unitDict   = util_yaml.load(configPath)
        unitDict   = util_yaml.process(unitDict, unitDict)

def convert(*args):

    '''
    Converts input relative to default unit, or between two units.

    Input(s): value, quantity, unitA, unitB (optional)
    Output(s): value
    '''

    value    = args[0]
    quantity = args[1]
    unitA    = args[2]

    # if (quantity == "temperature"):
        # value = convert_temp(args)
        # return value

    if (len(args) == 3):
        
        if (quantity and unitA):
            
            # Need error handling here for bad key
            factorA = unitDict[quantity][unitA]
            
            # Evaluate arithmetic operations, if necessary
            factorA = eval(str(factorA))

            value *= factorA

            # Unit A converted to default unit
            return value

    elif (len(args) == 4):

        unitB = args[3]

        if (quantity and unitA and unitB):
            
            # Need error handling here for bad key
            factorA = unitDict[quantity][unitA]
            factorB = unitDict[quantity][unitB]

            # Evaluate arithmetic operations, if necessary
            factorA = eval(str(factorA))
            factorB = eval(str(factorB))

            factorC = factorA / factorB
            value  *= factorC

            # Unit A converted to unit B
            return value

    else:
        
        # If unit is not specified or nondimensional 
        return 1.0

def temp_shit():

    # THINK about adding a temp function. or storing function handles for different conversions in the YML? who knows
    pass

if __name__ == "__main__":

    # Standalone execution
    pass