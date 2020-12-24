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
from scipy import integrate
import matplotlib.pyplot as plt

addPath = Path(__file__).parent / "../util"
sys.path.append(str(addPath.resolve()))

# Project modules
import util_unit
util_unit.config()

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
                # motor name, diameter [mm], length [mm], delays, prop. mass [kg], tot. mass [kg], manufacturer
                diameter  = float(words[1]) 
                length    = float(words[2])
                propMass  = float(words[4])
                totalMass = float(words[5])

            elif len(words) == 2: 
                
                # Process thrust data lines:
                # time [sec], thrust [N]
                time   = np.append(time,   float(words[0]))
                thrust = np.append(thrust, float(words[1]))

    # Convert units
    diameter = util_unit.convert(diameter, "length", "mm")
    length   = util_unit.convert(length,   "length", "mm")

    # Generate mass curve
    thrustNorm = thrust / thrust.max()
    alpha      = propMass / np.trapz(thrustNorm, time) # Scaling factor
    massFlow   = alpha * thrustNorm
    mass       = totalMass - integrate.cumtrapz(massFlow, time)
    mass       = np.insert(mass, 0, totalMass) # t=0 mass @ totalMass

    #-----------------------------------------------------#
    fig, ax = plt.subplots()
    ax.plot(time, thrust, label="Thrust [N]")
    ax.plot(time, mass*1e3, label="Motor Mass [g]")
    ax.set_xlabel("Time [sec]")
    ax.legend()
    fig.show()
    pdb.set_trace()

def load_rse(propPath):
    pass

if __name__ == "__main__":

    propPath = sys.argv[1]
    load(propPath)
