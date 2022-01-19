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

latitude = 0.0
altitude = 0.0
geodetic.init(latitude, altitude)

alt  = np.arange(0, 1e5, 1e2)
grav = np.empty(len(alt))

for iAlt in range(len(alt)):

    test.set_state_data("linPosZ", alt[iAlt])
    geodetic.update()
    grav[iAlt] = test.get_state_data("gravity")

# Visualization
fig, ax = plt.subplots()
ax.plot(grav, alt)
plt.show()

# %%
