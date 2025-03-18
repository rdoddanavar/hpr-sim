# System modules
import sys
import os
import shutil
import pathlib
import math
import numpy as np
import matplotlib.pyplot as plt

# Path modifications
paths = ["../../build/src", "../preproc", "../postproc", "../util"]

for item in paths:
    addPath = pathlib.Path(__file__).parent / item
    sys.path.append(str(addPath.resolve()))

# Project modules
import preproc_aerodynamics
import postproc_flight
import model

#------------------------------------------------------------------------------#

# Path setup
rootPath   = pathlib.Path(__file__).parent.parent.parent
inputPath  = rootPath / "input"
outputPath = rootPath / "output" / "test_aerodynamics"

if os.path.exists(outputPath):
    shutil.rmtree(outputPath)

os.mkdir(outputPath)

# Create model instances
test         = model.Test()
aerodynamics = model.Aerodynamics()
telem        = model.Telem()

# Set model dependencies
aerodynamics.add_deps([test])

# Initialize state from top-level model
telem.init(outputPath.as_posix(), "# test_aerodynamics.py", 3)
aerodynamics.init_state(telem)

# Initialize models
stateFields = ["linVelZ", "linVelY", "linVelX", "speedSound", "density", "isBurnout"]
test.init(stateFields)

(machData, alphaData, aeroData) = preproc_aerodynamics.load_csv(inputPath / "patriot_aerodynamics.csv")
refArea = 12.566/144.0 # ft^2
aerodynamics.init(refArea, machData, alphaData, aeroData["cpTotal"], aeroData["clPowerOff"], aeroData["cdPowerOff"], aeroData["clPowerOn"], aeroData["cdPowerOn"])

#------------------------------------------------------------------------------#

# Test model - alpha sweep
alphaRng = np.deg2rad(np.arange(0, 15.25, 0.25))
mach = 0.01
speedSound = 343
density = 1.225
cd = np.empty(len(alphaRng))
nCol = 16

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

    aerodynamics.update()
    telem.update()

    cd[iAlpha] = test.get_state_data("dragCoeff")

fig, ax = plt.subplots()
ax.plot(np.rad2deg(alphaData[0:nCol]), aeroData["cdPowerOff"][0:nCol], 'o', label="Data")
ax.plot(np.rad2deg(alphaRng), cd, label="Interpolation")
ax.set_xlabel("Alpha [deg]")
ax.set_ylabel("Drag Coeff [-]")
ax.set_title("Aero Model Interpolation: Alpha")
ax.legend()
fig.savefig(outputPath / "drag_coeff_alpha_sweep.png", bbox_inches="tight")

#------------------------------------------------------------------------------#

# Test model - mach sweep
machRng = np.arange(0, 5, 0.01)
alpha = 0.0
speedSound = 343
density = 1.225
cd = np.empty(len(machRng))
nRow = 500

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

    aerodynamics.update()
    telem.update()

    cd[iMach] = test.get_state_data("dragCoeff")

fig, ax = plt.subplots()
idx = np.arange(0,nRow*nCol,nCol)
ax.plot(machData[idx], aeroData["cdPowerOff"][idx], 'o', label="Data")
ax.plot(machRng, cd, label="Interpolation")
ax.set_xlabel("Mach [-]")
ax.set_ylabel("Drag Coeff [-]")
ax.set_title("Aero Model Interpolation: Mach")
ax.legend()
fig.savefig(outputPath / "drag_coeff_mach_sweep.png", bbox_inches="tight")
