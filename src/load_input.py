# /src/load_input.py

# Installed modules

import sys  # System Utilities
import pdb  # Python debugger
import yaml # YAML parser

# Path modifications
sys.path.insert(0, "../bin/")

# Program modules

import input # Input classes 

inputDir = "../input/"
fileName = "input.yaml"
filePath = inputDir + fileName

stream = open(filePath,'r')
raw    = yaml.load(stream)

value = raw["sim"]["numMC"]
obj   = input.Param(value)

print(obj.value)

pdb.set_trace()