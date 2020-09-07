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

# Program modules
import util_yaml

def config():

    '''
    Parses YAML config file, returns dict of unit conversion factors. 
    '''

    configPath = "../util/config_unit.yaml"
    unitDict   = util_yaml.load(configPath)
    unitDict   = util_yaml.process(unitDict, unitDict)

    return unitDict

def convert(*args):

    '''
    Provides conversion factor relative to default unit, or between two units.
    '''

    unitDict = args[0]
    quantity = args[1]
    unitA    = args[2]

    if (len(args) == 3):
        
        if (quantity and unitA):
            
            # Need error handling here for bad key
            factorA = unitDict[quantity][unitA]

            # Unit A relative to default unit
            return factorA 

    elif (len(args) == 4):

        unitB = args[3]

        if (quantity and unitA and unitB):
            
            # Need error handling here for bad key
            factorA = unitDict[quantity][unitA]
            factorB = unitDict[quantity][unitB]
            factorC = factorA / factorB

            # Unit A relative to unit B
            return factorC

    # If unit is not specified or nondimensional 
    return 1.0

def temp_shit():

    # THINK about adding a temp function. or storing function handles for different conversions in the YML? who knows
    pass

if __name__ == "__main__":

    # Standalone execution
    pass