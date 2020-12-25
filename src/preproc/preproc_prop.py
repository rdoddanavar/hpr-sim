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
import xml.etree.ElementTree as ET
import matplotlib.pyplot as plt

addPath = Path(__file__).parent / "../util"
sys.path.append(str(addPath.resolve()))

# Project modules
import util_unit
util_unit.config() # remove later when 

def load(propPath):
    
    ext = Path(propPath).suffix

    if ext == ".eng":
        load_eng(propPath)
    elif ext == ".rse":
        load_rse(propPath)
    else:
        raise ValueError('Unknown engine file extension', ext)

def load_eng(propPath):
    
    # Initialize arrays; implicit (0,0) point must be created
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
                time   = np.append(   time, float(words[0]) )
                thrust = np.append( thrust, float(words[1]) )

    # Convert units
    diameter = util_unit.convert( diameter, "length", "mm" )
    length   = util_unit.convert(   length, "length", "mm" )

    # Generate mass curve
    thrustNorm = thrust / thrust.max()
    alpha      = propMass / np.trapz(thrustNorm, time) # Scaling factor
    massFlow   = alpha * thrustNorm
    mass       = totalMass - integrate.cumtrapz(massFlow, time)
    mass       = np.insert(mass, 0, totalMass) # t=0 mass @ totalMass

def load_rse(propPath):
    
    tree = ET.parse(propPath)
    root = tree.getroot()

    engine    = root[0][0]
    diameter  = float(engine.attrib["dia"])
    length    = float(engine.attrib["len"])
    propMass  = float(engine.attrib["propWt"])
    totalMass = float(engine.attrib["initWt"])

    # Initialize arrays; (0,0) point included in *.rse file
    time   = np.array([])
    thrust = np.array([])
    mass   = np.array([])

    engineData = engine[1]

    for point in engineData:

        time   = np.append(time,   float(point.attrib['t']))
        thrust = np.append(thrust, float(point.attrib['f']))
        mass   = np.append(mass,   float(point.attrib['m']))

    # Convert units
    diameter  = util_unit.convert(  diameter, "length", "mm" )
    length    = util_unit.convert(    length, "length", "mm" )
    propMass  = util_unit.convert(  propMass,   "mass",  "g" ) # *.rse file gives grams
    totalMass = util_unit.convert( totalMass,   "mass",  "g" ) # " "
    mass      = util_unit.convert(      mass,   "mass",  "g" ) # " "

    # Mass curve should reflect overall engine mass; *.rse file gives propellant mass only
    mass = totalMass - (propMass - mass)

if __name__ == "__main__":

    propPath = sys.argv[1]
    load(propPath)
