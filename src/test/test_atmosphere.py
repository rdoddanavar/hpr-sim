# System modules
import sys
import os
import shutil
import pathlib
import numpy as np
import matplotlib.pyplot as plt

# Path modifications
paths = ["../../build/src", "../postproc", "../util/"]

for item in paths:
    addPath = pathlib.Path(__file__).parent / item
    sys.path.append(str(addPath.resolve()))

# Project modules
import postproc_flight
import model

#------------------------------------------------------------------------------#

# Path setup
rootPath   = pathlib.Path(__file__).parent.parent.parent
inputPath  = rootPath / "input" / "unit_test"
outputPath = rootPath / "output" / "test_atmosphere"

if os.path.exists(outputPath):
    shutil.rmtree(outputPath)

os.mkdir(outputPath)

# Create model instances
test       = model.Test()
atmosphere = model.Atmosphere()
telem      = model.Telem()

# Set model dependencies
atmosphere.add_deps([test])

# Initialize state from top-level model
telem.init(outputPath.as_posix(), "# test_atmosphere.py", 3)
atmosphere.init_state(telem)

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
    telem.update()

    temp[iAlt]    = test.get_state_data("temperature")
    press[iAlt]   = test.get_state_data("pressure")
    density[iAlt] = test.get_state_data("density")
    dynVisc[iAlt] = test.get_state_data("dynamicViscosity")

# Export data to *.mat
telem.finalize()
postproc_flight.npy_to_mat(outputPath / "telem.npy")

# Visualization
plt.rcParams['text.usetex'] = True

fig, ax = plt.subplots()
ax.plot(temp, alt)
isotherms = [0.0e3, 11.0e3, 20.0e3, 32.0e3, 47.0e3, 51.0e3, 71.0e3]

for layer in isotherms:
    hline = ax.axhline(layer, linestyle=':')

hline.set_label("Isotherm")
ax.set_title("Temperature Profile")
ax.set_xlabel("Air Temperature [$K$]")
ax.set_ylabel("Geopotential Altitude [$m$]")
ax.legend()
fig.savefig(outputPath / "temperature.png", bbox_inches="tight")

fig, ax = plt.subplots()
ax.plot(press, alt)
ax.set_title("Pressure Profile")
ax.set_xlabel("Air Pressure [$Pa$]")
ax.set_ylabel("Geopotential Altitude [$m$]")
fig.savefig(outputPath / "pressure.png", bbox_inches="tight")

fig, ax = plt.subplots()
ax.plot(density, alt)
ax.set_title("Density Profile")
ax.set_xlabel(r"Air Density [$\frac{kg}{m^3}$]")
ax.set_ylabel("Geopotential Altitude [$m$]")
fig.savefig(outputPath / "density.png", bbox_inches="tight")

fig, ax = plt.subplots()
ax.plot(temp, dynVisc)
ax.set_title("Dynamic Viscosity")
ax.set_xlabel("Air Temperature [$K$]")
ax.set_ylabel(r"Dynamic Viscosity [$\frac{kg}{m \cdot s}$]")
fig.savefig(outputPath / "dynamic_viscosity.png", bbox_inches="tight")
