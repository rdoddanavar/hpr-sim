#%%

# System modules
import sys
import pathlib
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

printFigs = True
figDest   = "../../doc/src/fig/"

# Create model instances
test       = model.Test()
atmosphere = model.Atmosphere()

# Set model dependencies
atmosphere.add_dep(test)

# Initialize state from top-level model
atmosphere.init_state()

# Initialize models
stateFields = ["altitudeGP", "gravity0"]
test.init(stateFields)
test.set_state_data("altitudeGP", 0.0)
test.set_state_data("gravity0", 9.80665)

temp0  = 288.15
press0 = 101325.0
atmosphere.init(temp0, press0)

# Test model
alt     = np.arange(0, 80e3, 1e3)
temp    = np.empty(len(alt))
press   = np.empty(len(alt))
density = np.empty(len(alt))
dynVisc = np.empty(len(alt))

for iAlt in range(len(alt)):

    test.set_state_data("altitudeGP", alt[iAlt])
    atmosphere.update()

    temp[iAlt]    = test.get_state_data("temperature")
    press[iAlt]   = test.get_state_data("pressure")
    density[iAlt] = test.get_state_data("density")
    dynVisc[iAlt] = test.get_state_data("dynamicViscosity")

# Visualization
plt.rcParams['text.usetex'] = True

fig, ax = plt.subplots()
ax.plot(temp, alt)
tempLayers = [0.0e3, 11.0e3, 20.0e3, 32.0e3, 47.0e3, 51.0e3, 71.0e3]

for layer in tempLayers:
    ax.axhline(layer, linestyle=':')

ax.set_title("Temperature Profile")
ax.set_xlabel("Air Temperature [$K$]")
ax.set_ylabel("Geopotential Altitude [$m$]")

fig, ax = plt.subplots()
ax.plot(press, alt)
ax.set_title("Pressure Profile")
ax.set_xlabel("Air Pressure [$Pa$]")
ax.set_ylabel("Geopotential Altitude [$m$]")

fig, ax = plt.subplots()
ax.plot(density, alt)
ax.set_title("Density Profile")
ax.set_xlabel(r"Air Density [$\frac{kg}{m^3}$]")
ax.set_ylabel("Geopotential Altitude [$m$]")

fig, ax = plt.subplots()
ax.plot(temp, dynVisc)
ax.set_title("Dynamic Viscosity")
ax.set_xlabel("Air Temperature [$K$]")
ax.set_ylabel(r"Dynamic Viscosity [$\frac{kg}{m \cdot s}$]")

plt.show()

# %%
