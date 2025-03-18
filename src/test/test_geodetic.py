# System modules
import sys
import os
import shutil
import pathlib
import math
import numpy as np
import matplotlib.pyplot as plt

# Path modifications
paths = ["../../build/src", "../postproc", "../util"]

for item in paths:
    addPath = pathlib.Path(__file__).parent / item
    sys.path.append(str(addPath.resolve()))

# Project modules
import postproc_flight
import model

#------------------------------------------------------------------------------#

# Path setup
rootPath   = pathlib.Path(__file__).parent.parent.parent
inputPath  = rootPath / "input"
outputPath = rootPath / "output" / "test_geodetic"

if os.path.exists(outputPath):
    shutil.rmtree(outputPath)

os.mkdir(outputPath)

# Create model instances
test     = model.Test()
geodetic = model.Geodetic()
telem    = model.Telem()

# Set model dependencies
geodetic.add_deps([test])

# Initialize state from top-level model
telem.init(outputPath.as_posix(), "# test_geodetic.py", 3)
geodetic.init_state(telem)

# Initialize models
stateFields = ["linPosZ"]
test.init(stateFields)

latDeg   = 22.5
latitude = math.radians(latDeg)
altitude = 0.0
geodetic.init(latitude, altitude)

alt   = np.arange(0, 1e5, 1e2)
altGP = np.empty(len(alt))
grav  = np.empty(len(alt))

for iAlt in range(len(alt)):

    test.set_state_data("linPosZ", alt[iAlt])
    geodetic.update()
    telem.update()

    altGP[iAlt] = test.get_state_data("altitudeGP")
    grav[iAlt]  = test.get_state_data("gravity")

altDiff     = np.zeros(len(alt))
altDiff[1:] = 100.0*(alt[1:] - altGP[1:])/alt[1:]

# Export data to *.mat
telem.finalize()
postproc_flight.npy_to_mat(outputPath / "telem.npy")

# Visualization
plt.rcParams['text.usetex'] = True

fig, ax = plt.subplots()
ax.plot(alt, altDiff)
ax.set_title("Geopotential Altitude Correction")
ax.set_xlabel("Geometric Altitude [$m$]")
ax.set_ylabel("Altitude Correction [$\\%$]")
fig.savefig(outputPath / "altitude_correction.png", bbox_inches="tight")

fig, ax = plt.subplots()
ax.plot(alt, grav, label=f"$\\phi = {latDeg}\\deg$")
ax.set_title("Gravity at Altitude")
ax.set_xlabel("Geometric Altitude [$m$]")
ax.set_ylabel("Gravity [$m/s^2$]")
ax.legend()
fig.savefig(outputPath / "gravity.png", bbox_inches="tight")
