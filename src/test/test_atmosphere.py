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
test       = model.Test()
atmosphere = model.Atmosphere()

# Set model dependencies
atmosphere.add_dep(test)

# Initialize state from top-level model
atmosphere.init_state()

# Initialize models

stateFields = ["altitudeMSL", "gravity0"]
test.init(stateFields)
test.set_state_data("altitudeMSL", 0.0)
test.set_state_data("gravity0", 9.81)

temp0  = 288.15
press0 = 101325.0
atmosphere.init(temp0, press0)

alt   = np.arange(0, 80e3, 1e3)
temp  = np.empty(len(alt))
press = np.empty(len(alt))

for iAlt in range(len(alt)):

    test.set_state_data("altitudeMSL", alt[iAlt])
    atmosphere.update()
    temp[iAlt]  = test.get_state_data("temperature")
    press[iAlt] = test.get_state_data("pressure")

# Visualization
fig, ax = plt.subplots()
ax.plot(temp, alt)
ax.grid(True)
plt.show()

fig, ax = plt.subplots()
ax.plot(press, alt)
ax.grid(True)
plt.show()

# %%
