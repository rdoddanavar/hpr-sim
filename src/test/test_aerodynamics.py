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
import model

#------------------------------------------------------------------------------#

# Create model instances
test = model.Test()
aero = model.Aerodynamics()

# Set model dependencies
aero.add_dep(test)

# Initialize state from top-level model
aero.init_state()

# Initialize models
stateFields = ["mach", "alpha"]
test.init(stateFields)

inputPath = "/home/roshan/Documents/hpr-sim/patriot/CDDataFile/"
(machData, alphaData, aeroData) = preproc_aerodynamic.load(inputPath)

refArea = 12.566/144.0 # ft^2
aero.init(refArea, machData, alphaData, aeroData["cdPowerOff"], aeroData["cdPowerOn"], aeroData["clPowerOff"], aeroData["clPowerOn"], aeroData["cpTotal"])

# Test model
cd = np.empty(len(alphaData))
iMach = 0

for iAlpha in range(len(alphaData)):

    test.set_state_data("mach", machData[iMach])
    test.set_state_data("alpha", alphaData[iAlpha])
    aero.update()

    cd[iAlpha] = test.get_state_data("dragCoeff")

#------------------------------------------------------------------------------#

fig, ax = plt.subplots()
ax.plot(machData, aeroData["cdPowerOff"][:, [0]], label="PowerOff")
ax.plot(machData, aeroData["cdPowerOn"][:, [0]], label="PowerOn")
ax.set_title("RASAero Raw Data")
ax.legend()

fig, ax = plt.subplots()
ax.plot(alphaData, aeroData["cdPowerOff"][0], 'o')
ax.plot(alphaData, cd)
ax.set_title("Model Data Interpolation")

plt.show()

# %%
