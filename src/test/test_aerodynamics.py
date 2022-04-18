#%%

# System modules
import sys
import pathlib
import math
import numpy as np
import matplotlib.pyplot as plt

# Path modifications
paths = ["../../build/src", "../../src/preproc"]

for item in paths:
    addPath = pathlib.Path(__file__).parent / item
    sys.path.append(str(addPath.resolve()))

# Project modules
import preproc_aerodynamics
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
stateFields = ["linVelZ", "linVelY", "linVelX", "speedSound", "density", "isBurnout"]
test.init(stateFields)

inputPathRel = "../../input/patriot_aerodynamics.csv"
inputPath    = pathlib.Path(__file__).parent / inputPathRel
inputPath    = inputPath.resolve()
(machData, alphaData, aeroData) = preproc_aerodynamics.load_csv(inputPath)

refArea = 12.566/144.0 # ft^2
aero.init(refArea, machData, alphaData, aeroData["cpTotal"], aeroData["clPowerOff"], aeroData["cdPowerOff"], aeroData["clPowerOn"], aeroData["cdPowerOn"])

# Test model - alpha sweep
alphaRng = np.deg2rad(np.arange(0, 15.25, 0.25))
mach = 0.01
speedSound = 343
density = 1.225
cd = np.empty(len(alphaRng))

for iAlpha in range(len(alphaRng)):

    linVel  = mach*speedSound
    linVelZ = linVel*math.cos(alphaRng[iAlpha])
    linVelY = 0.0
    linVelX = linVel*math.sin(alphaRng[iAlpha])

    test.set_state_data("linVelZ", linVelZ)
    test.set_state_data("linVelY", linVelY)
    test.set_state_data("linVelX", linVelX)
    test.set_state_data("speedSound", speedSound)
    test.set_state_data("density", density)
    test.set_state_data("isBurnout", 1.0)

    aero.update()

    cd[iAlpha] = test.get_state_data("dragCoeff")

fig, ax = plt.subplots()
ax.plot(np.arange(0,16,1), aeroData["cdPowerOff"][0:16], 'o')
ax.plot(np.rad2deg(alphaRng), cd)
ax.set_title("Model Data Interpolation")

#------------------------------------------------------------------------------#

# Test model - mach sweep
machRng = np.arange(0, 5, 0.01)
alpha = 0.0
speedSound = 343
density = 1.225
cd = np.empty(len(machRng))

for iMach in range(len(machRng)):

    linVel  = machRng[iMach]*speedSound
    linVelZ = linVel*math.cos(alpha)
    linVelY = 0.0
    linVelX = linVel*math.sin(alpha)

    test.set_state_data("linVelZ", linVelZ)
    test.set_state_data("linVelY", linVelY)
    test.set_state_data("linVelX", linVelX)
    test.set_state_data("speedSound", speedSound)
    test.set_state_data("density", density)
    test.set_state_data("isBurnout", 1.0)

    aero.update()

    cd[iMach] = test.get_state_data("dragCoeff")

fig, ax = plt.subplots()
ax.plot(machRng, cd)
ax.set_title("Model Data Interpolation")

plt.show()

# %%
