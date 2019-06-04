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
    fileDict = yaml.safe_load(stream)

    return fileDict

def processInput(fileDict):

    """
    Processes pyYAML output; resolves references and evaluates arithmetic expressions. 
    """

    # Do initial pass to populate every "value"?

    for group in fileDict.keys():
        for param in fileDict[group].keys():

            # Fix condition when not a dict 

            for field in fileDict[group][param].keys():

                value = fileDict[group][param][field]

                print('\n')
                print('.'.join([group, param, field]) + ':')
                print("original: ", value)

                if (isinstance(value, str)):

                    while ("ref" in value):

                        # Parse value for target
                        idxA   = value.find("ref") + 4
                        idxB   = value[idxA:].find(')') + idxA
                        target = value[idxA:idxB]

                        # Get keys to target
                        current = [group, param, field]
                        target  = target.split('.')
                        target  = getTarget(fileDict, current, target)

                        if (target == current):

                            # Circular reference
                            print("CIRCULAR REFERENCE")
                            return fileDict

                        targetValue = fileDict[target[0]][target[1]][target[2]]
                        refStr      = "ref(" + value[idxA:idxB] + ')'

                        # Value may be float, must cast to string
                        value = value.replace(refStr, str(targetValue))

                        print("mod: ", value)

                    # Evaluate any arithmetic expressions
                    value = mathEval(value)
                    print("calc: ", value)

                    # Reassign field 
                    fileDict[group][param][field] = value

                    print("assigned: ", value)

    return fileDict

def getTarget(fileDict, current, target):

    """
    Given current position in YAML dict, give absolute keys to relative target.
    """

    groupName = fileDict.keys()
    paramName = fileDict[current[0]].keys()

    if (len(target) == 3):

        # Target defined; no action
        group = target[0]
        param = target[1]
        field = target[2]

    elif (len(target) == 2):

        if (target[0] in groupName):

            # Assume field as "value"
            group = target[0]
            param = target[1]
            field = "value"

        else:

            # Assume current group
            group = current[0]
            param = target[0]
            field = target[1]

    elif (len(target) == 1):

        if (target[0] in paramName):

            # Assume current group and field as "value"
            group = current[0]
            param = target[0]
            field = "value"

        else:
            # Assume current group and param
            group = current[0]
            param = current[1]
            field = target[0]

    target = [group, param, field]

    return target

def mathEval(value):

    """
    Evaluates arithmetic string expressions for a limited set of operators. 
    """

    # Allowed arithmetic operators + whitespace + parentheses
    token = ['+', '-', '*', '/', "**", ' ', '(', ')']
    test  = value

    for char in token:
        test = test.replace(char, "")

    if (test.isnumeric()):

        # Evaluate original expression if valid
        return eval(value)

    else:
        return value

if __name__ == "__main__":
    
    pass

