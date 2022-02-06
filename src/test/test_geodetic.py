#%%

# System modules
import sys
import pathlib
import math
import numpy as np
import matplotlib.pyplot as plt

# Path modifications
paths = ["../../build/src"]

for item in paths:
    addPath = pathlib.Path(__file__).parent / item
    sys.path.append(str(addPath.resolve()))

# Project modules
import model

#------------------------------------------------------------------------------#

# Create model instances
test     = model.Test()
geodetic = model.Geodetic()

# Set model dependencies
geodetic.add_dep(test)

# Initialize state from top-level model
geodetic.init_state()

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
    altGP[iAlt] = test.get_state_data("altitudeGP")
    grav[iAlt]  = test.get_state_data("gravity")

altDiff     = np.zeros(len(alt))
altDiff[1:] = 100.0*(alt[1:] - altGP[1:])/alt[1:]

# Visualization
plt.rcParams['text.usetex'] = True

fig, ax = plt.subplots()
ax.plot(alt, altDiff)
ax.set_title("Geopotential Altitude Correction")
ax.set_xlabel("Geometric Altitude [$m$]")
ax.set_ylabel("Altitude Correction [$\%$]")
ax.legend()

fig, ax = plt.subplots()
ax.plot(grav, alt, label="$\phi = " + str(latDeg) + " \deg$")
ax.set_title("Gravity at Altitude")
ax.set_xlabel("Gravity [$m/s^2$]")
ax.set_ylabel("Geometric Altitude [$m$]")
ax.legend()

plt.show()

# %%
