#%%

# System modules
import sys
import pathlib
import numpy as np
import matplotlib.pyplot as plt

# Path modifications
paths = ["../../build/src", "../../src/preproc"]

for item in paths:
    addPath = pathlib.Path(__file__).parent / item
    sys.path.append(str(addPath.resolve()))

# Project modules
import preproc_aero

#------------------------------------------------------------------------------#

inputPath = "/home/roshan/Documents/hpr-sim/patriot/CDDataFile/"
(machData, alphaData, cpData, clDataPowerOff, cdDataPowerOff, clDataPowerOn, cdDataPowerOn) = preproc_aero.load(inputPath)

fig, ax = plt.subplots()
ax.plot(alphaData, cdDataPowerOff[0])