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
'''

# System modules
import pathlib

# Project modules
import util_yaml
import util_misc

# Module variables
configUnit = None

#------------------------------------------------------------------------------#

def config():

    '''
    Parses YAML config file, creates global dict of unit conversion factors.

    Input(s): <none> \n
    Outputs(s): <none>
    '''

    global configUnit # Necessary for reassignment

    if configUnit is None:

        subdir     = "_internal" if util_misc.is_bundled() else "."
        configPath = pathlib.Path(subdir) / "config/config_unit.yml"
        configUnit = util_yaml.load(configPath)
        util_yaml.process(configUnit)

#------------------------------------------------------------------------------#

def convert(*args):

    '''
    Converts input relative to default unit, or between two units.

    Input(s): value (float), quantity (str), unitA (str), unitB (str) [opt.] \n
    Output(s): value (float)
    '''

    value    = args[0]
    quantity = args[1]
    unitA    = args[2]

    if unitA == "default":
        unitA = configUnit["default"][quantity]

    if len(args) == 3:
            
        if quantity == "temperature":
            value = convert_temp(value, unitA)

        else:
            
            # TODO: Need error handling here for bad key
            factorA = configUnit[quantity][unitA]

            value *= factorA

    elif len(args) == 4:

        unitB = args[3]

        if unitB == "default":
            unitB = configUnit["default"][quantity]
            
        if quantity == "temperature":
            value = convert_temp(value, unitA, unitB)
        
        else:
            
            # TODO: Need error handling here for bad key
            factorA = configUnit[quantity][unitA]
            factorB = configUnit[quantity][unitB]

            factorC = factorA/factorB
            value  *= factorC
        
    # Original value returned if unit is not specified or nondimensional 
    return value

#------------------------------------------------------------------------------#

def convert_temp(*args):

    '''
    Converts temperature relative to default unit (K), or between two units.

    Input(s): value (float), unitA (str), unitB (str) [opt.] \n
    Output(s): value (float)
    '''

    value    = args[0]
    quantity = "temperature"
    unitA    = args[1]

    factorA = configUnit[quantity][unitA][0]
    offsetA = configUnit[quantity][unitA][1]

    # Necessary b/c util_yaml.process() does not simplify temperature factors
    factorA = util_yaml.math_eval(str(factorA))
    offsetA = util_yaml.math_eval(str(offsetA))
    
    value = value*factorA + offsetA

    if len(args) == 3:

        unitB = args[2]

        factorB = configUnit[quantity][unitB][0]
        offsetB = configUnit[quantity][unitB][1]

        # Necessary b/c util_yaml.process() does not simplify temperature factors
        factorB = util_yaml.math_eval(str(factorB))
        offsetB = util_yaml.math_eval(str(offsetB))

        value = (value - offsetB)/factorB
    
    return value

#------------------------------------------------------------------------------#

if __name__ == "__main__":
    pass
else:
    config()
