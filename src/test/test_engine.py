#%%

# System modules
import sys
import pathlib
import numpy as np
import matplotlib.pyplot as plt

# Path modifications
paths = ["../../build/src", "../preproc"]

for item in paths:
    addPath = pathlib.Path(__file__).parent / item
    sys.path.append(str(addPath.resolve()))

# Project modules
import preproc_engine
import model

#------------------------------------------------------------------------------#

#printFigs = True
#figDest   = "../../doc/src/fig/"

# Create model instances
test   = model.Test()
engine = model.Engine()

# Set model dependencies
engine.add_dep(test)

# Initialize state from top-level model
engine.init_state()

# Initialize models
stateFields = ["time"]
test.init(stateFields)

enginePath = "/home/roshan/Git/hpr-sim/input/engine/AeroTech_J450DM.eng"
timeData, thrustData, massData = preproc_engine.load(enginePath)
engine.init(timeData, thrustData, massData)

# Test model
time    = np.arange(0, 3, 0.01)
thrust  = np.empty(len(time))
massEng = np.empty(len(time))

for iTime in range(len(time)):

    test.set_state_data("time", time[iTime])
    engine.update()

    thrust[iTime]  = test.get_state_data("thrust")
    massEng[iTime] = test.get_state_data("massEng")

# Visualization
fig, ax = plt.subplots()
ax.plot(time, thrust)
ax.set_title("Thrust Profile")
ax.set_xlabel("Time [s]")
ax.set_ylabel("Thrust [N]")

fig, ax = plt.subplots()
ax.plot(time, massEng)
ax.set_title("Engine Mass Profile")
ax.set_xlabel("Time [s]")
ax.set_ylabel("Mass [kg]")

# %%