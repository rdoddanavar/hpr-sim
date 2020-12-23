'''
High Power Rocketry - Flight Simulation
MIT License
Copyright (C) 2019 Roshan Doddanavar
https://rdoddanavar.github.io

Path:
    hpr-sim/src/preproc/preproc_prop.py
Created:
    2020-12-22
Type:
    Python3 module
Description:
    < >
Functions:
    < >
Classes:
    < >
Dependencies:
    < >
'''

# System modules
import sys
import pdb
from pathlib import Path
import numpy as np
import matplotlib.pyplot as plt

def load(propPath):
    
    ext = Path(propPath).suffix

    if ext == ".eng":
        load_eng(propPath)
    elif ext == ".rse":
        load_rse(propPath)
    else:
        raise ValueError('Unknown engine file extension', ext)

def load_eng(propPath):
    
    # Initialize assumed (0,0) point
    time   = np.array([0])
    thrust = np.array([0])

    with open(propPath, 'r') as engFile:
        for line in engFile.read().splitlines():
            
            if ';' in line:
                continue # Skip header comments
            
            words = line.strip().split(' ')

            if len(words) > 2:
                
                # Process info line:
                # motor name, diameter [mm], length [mm], delays, prop. weight [kg], tot. weight [kg], manufacturer
                diameter    = float(words[1]) 
                length      = float(words[2])
                propWeight  = float(words[4])
                totalWeight = float(words[5])

            elif len(words) == 2: 
                
                # Process thrust data lines:
                # time [sec], thrust [N]
                time   = np.append(time,   float(words[0]))
                thrust = np.append(thrust, float(words[1]))

    plt.plot(time, thrust)
    plt.show()

def load_rse(propPath):
    pass

if __name__ == "__main__":

    propPath = sys.argv[1]
    load(propPath)
