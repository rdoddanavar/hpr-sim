'''
High Power Rocketry - Flight Simulation
MIT License
Copyright (c) 2019 Roshan Doddanavar
https://rdoddanavar.github.io

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
from pathlib import Path

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

        configPath = Path(__file__).parent / "../../config/config_unit.yaml"
        configPath = str(configPath.resolve())
        unitDict   = util_yaml.load(configPath)

def convert(*args):

    '''
    Converts input relative to default unit, or between two units.

    Input(s): value, quantity, unitA, unitB (optional)
    Output(s): value
    '''

    value    = args[0]
    quantity = args[1]
    unitA    = args[2]

    if (len(args) == 3):
        
        if (quantity and unitA):
            
            if (quantity == "temperature"):
                value = convert_temp(value, unitA)

            else:
                
                # Need error handling here for bad key
                factorA = unitDict[quantity][unitA]
                
                # Evaluate arithmetic operations, if necessary
                factorA = eval(str(factorA))

                value *= factorA

    elif (len(args) == 4):

        unitB = args[3]

        if (quantity and unitA and unitB):
            
            if (quantity == "temperature"):
                value = convert_temp(value, unitA, unitB)
            
            else:
                
                # Need error handling here for bad key
                factorA = unitDict[quantity][unitA]
                factorB = unitDict[quantity][unitB]

                # Evaluate arithmetic operations, if necessary
                factorA = eval(str(factorA))
                factorB = eval(str(factorB))

                factorC = factorA/factorB
                value  *= factorC
        
    # Original value returned if unit is not specified or nondimensional 
    return value

def convert_temp(*args):

    '''
    Converts temperature relative to default unit (K), or between two units.

    Input(s): value, unitA, unitB (optional)
    Output(s): value
    '''

    value = args[0]
    unitA = args[1]

    factorA = unitDict["temperature"][unitA][0]
    offsetA = unitDict["temperature"][unitA][1]

    factorA = eval(str(factorA))
    offsetA = eval(str(offsetA))
    
    value = value*factorA + offsetA

    if (len(args) == 3):

        unitB = args[2]

        factorB = unitDict["temperature"][unitB][0]
        offsetB = unitDict["temperature"][unitB][1]

        factorB = eval(str(factorB))
        offsetB = eval(str(offsetB))

        value = (value - offsetB)/factorB
    
    return value

if __name__ == "__main__":

    # Standalone execution
    pass