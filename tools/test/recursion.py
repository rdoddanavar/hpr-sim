import sys
import pdb
import yaml

################
# EXAMPLE
def getpath(nested_dict, value, prepath=()):
    for k, v in nested_dict.items():
        path = prepath + (k,)
        if v == value: # found value
            return path
        elif hasattr(v, 'items'): # v is a dict
            p = getpath(v, value, path) # recursive call
            if p is not None:
                return p
################

def getValue(value, path):

    for key in path:
        value = value[key]

    return value

def setValue(original, path):

    if len(path) == 2:
        original[path[0]][path[1]] = "test"

    elif len(path) == 3:
        original[path[0]][path[1]][path[2]] = "test"

    return original

'''
def iterate(fileDict, path):

    for key, value in fileDict.items():

        path = path + [key]

        if isinstance(value, dict):
            
            p = iterate(value, path)

            print(p)

        else: 
            return path
'''

def iterate(original, fileDict, path, test):

    for key, value in fileDict.items():

        if test:
            path[-1] = key
        else: 
            test = True
            path = path + [key]

        if isinstance(value, dict):
            iterate(original, value, path, False)

        else: 
            
            test = setValue(original, path)
            print(test)
            print(path)
            print("")

filePath = "./test.yaml"
stream   = open(filePath, 'r')
fileDict = yaml.safe_load(stream)

iterate(fileDict, fileDict, [], False)

print(fileDict)

#print(getpath(fileDict, "three"))

# SEPARATE CONDITIONING FOR INPUT FILE
# Convert every single level key:value pair to include "value" field
# Generalize preprocessing and find-target functions

# clarify input file features
# plan structure
# get dict features










