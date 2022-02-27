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
import preproc_aerodynamic

#------------------------------------------------------------------------------#

inputPath = "/home/roshan/Documents/hpr-sim/patriot/CDDataFile/"
(machData, alphaData, aeroData) = preproc_aerodynamic.load(inputPath)

fig, ax = plt.subplots()
ax.plot(machData, aeroData["cdPowerOff"][:, [0]], label="PowerOff")
ax.plot(machData, aeroData["cdPowerOn"][:, [0]], label="PowerOn")
ax.legend()

fig, ax = plt.subplots()
ax.plot(alphaData, aeroData["cdPowerOff"][0])

plt.show()

# %%
