# /src/load_input.py

# Installed modules
import sys  # System utilities
import pdb  # Python debugger
import yaml # YAML parser

# Path modifications
sys.path.insert(0, "../bin/")

# Program modules
import input # Input classes 

#----------------------------#

# YAML parse
inputDir   = "../input/"
inputName  = "input.yaml"
configName = "param_config.yaml"

filePath  = inputDir + inputName
stream    = open(filePath,'r')
inputDict = yaml.safe_load(stream)

filePath   = inputDir + configName
stream     = open(filePath,'r')
configDict = yaml.safe_load(stream)  

# test
class Object:
    pass

inputObj = Object()

# Param assignment & validation
for group in inputDict.keys():
    for param in inputDict[group].keys():
        for field in inputDict[group][param].keys():

            value = inputDict[group][param][field]

            if (isinstance(value,float) or isinstance(value, int)):
                paramObj = input.Param(float(value))

            elif (isinstance(value,str)):
                paramObj = input.Name(value)

            else:
                print("Input error: invalid type\n")

            pdb.set_trace()

# None of this is correct buddy
# Remove constructor, only use default 
# Nah accept both cases; if not a dict, or if all fields provided
# isinstance(inputObj,dist)

pdb.set_trace()