"""
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
    hpr-sim/src/util/util_yaml.py
Created:
    2019-05-25
Type:
    Python3 module
Description:
    YAML utilities using pyYAML library.
Functions:
    load
    processInput
    getTarget
    mathEval
Classes:
    None
Dependencies:
    None
"""

# System modules
import sys  # System utilities
import pdb  # Python debugger
import yaml # YAML parser

def load(filePath):

    """
    Parses YAML document using filepath, returns dict.
    """

    stream   = open(filePath, 'r')
    yamlDict = yaml.safe_load(stream)

    return yamlDict

def process(yamlDict, subDict=None, path=[], first=True):

    """
    Processes pyYAML output; resolves references and evaluates arithmetic expressions. 
    """

    if subDict is None:
        subDict = yamlDict.copy()

    for key, value in subDict.items():

        if first:

            first = False
            path  = path + [key]

        else: 
            path[-1] = key

        if isinstance(value, dict):
            yamlDict = process(yamlDict, value, path)

        elif isinstance(value, str):

            while "ref" in value:

                # Parse value for target
                idxA   = value.find("ref(") + 4
                idxB   = value[idxA:].find(')') + idxA
                target = value[idxA:idxB]

                # Get keys to target
                target = target.split('.')
                nRel   = len(path) - len(target)
                target = path[:nRel] + target

                # Error handling: circular reference
                if target == path:
                    raise ValueError("CIRCULAR REFERENCE",value)

                # Error handling: invalid reference
                try:
                    targetValue = get_value(yamlDict, target)
                except:
                    raise KeyError("INVALID REFERENCE",value)

                # Value may be float, must cast to string
                refStr = "ref(" + value[idxA:idxB] + ')'
                value  = value.replace(refStr, str(targetValue))

            # Evaluate any arithmetic expressions & reassign field
            value    = math_eval(value)
            yamlDict = set_value(yamlDict, value, path)
    
    return yamlDict

def get_value(nested, path):

    '''
    Fetches value in arbitrarily nested dict given list of keys. 
    '''

    value = nested

    for key in path:
        value = value[key]

    return value

def set_value(nested, value, path):

    '''
    Sets value in arbitrarily nested dict given list of keys.
    '''

    sub = nested[path[0]]

    if isinstance(sub, dict):

        sub = set_value(sub, value, path[1:])
        nested[path[0]] = sub

        return nested

    else:

        nested[path[0]] = value

        return nested
    
def math_eval(value):

    """
    Evaluates arithmetic string expressions for a limited set of operators, returns float.

    Inputs(s): value
    Output(s): value
    """
    
    # Allowed: decimals, arithmetic operators, scientific notation, whitespace, parentheses
    # Prevents expressions from polluting workspaces
    token = ['.', '+', '-', '*', '/', "**", '%', '//', 'e', 'E', ' ', '(', ')']
    test  = value

    for char in token:
        test = test.replace(char, "")

    if test.isnumeric():
        return eval(value) # Evaluate original expression if valid
    else:
        return value

if __name__ == "__main__":

    # Standalone execution
    pass