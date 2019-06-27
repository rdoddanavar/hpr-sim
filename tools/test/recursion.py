import sys
import pdb
import yaml

def getValue(nested, path):

    value = nested

    for key in path:
        value = value[key]

    return value

def setValue(nested, value, path):

    '''
    Set value in arbitrarily nested dict given list of keys.
    '''

    sub = nested[path[0]]

    if isinstance(sub, dict):

        sub = setValue(sub, value, path[1:])
        nested[path[0]] = sub

        return nested

    else:

        nested[path[0]] = value

        return nested

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
            
            original = setValue(original, path)
            print(path)

    return original

filePath = "./test.yml"
stream   = open(filePath, 'r')
fileDict = yaml.safe_load(stream)

print(getValue(fileDict, ['group', 'b', 'max']))

print("")
print(fileDict)

fileDict = setValue(fileDict, 'five', ['group', 'b', 'max'])

print("")
print(fileDict)

pdb.set_trace()











