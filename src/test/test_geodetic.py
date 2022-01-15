# System modules
import sys
import pathlib
import numpy as np

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

alt  = [0e3, 1e3, 2e3, 3e3, 4e3, 5e3]
grav = np.empty(len(alt))

for iAlt in range(len(alt)):

    test.set_state_data("linPosZ", alt[iAlt])
    geodetic.update()
    grav[iAlt] = test.get_state_data("gravity")

    print(grav[iAlt])