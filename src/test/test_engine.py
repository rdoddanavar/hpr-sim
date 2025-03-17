# System modules
import sys
import os
import shutil
import pathlib
import numpy as np
import matplotlib.pyplot as plt

# Path modifications
paths = ["../../build/src", "../preproc", "../postproc"]

for item in paths:
    addPath = pathlib.Path(__file__).parent / item
    sys.path.append(str(addPath.resolve()))

# Project modules
import preproc_engine
import postproc_flight
import model

#------------------------------------------------------------------------------#

# Path setup
rootPath   = pathlib.Path(__file__).parent.parent.parent
inputPath  = rootPath / "input"
outputPath = rootPath / "output" / "test_engine"

if os.path.exists(outputPath):
    shutil.rmtree(outputPath)

os.mkdir(outputPath)

# Create model instances
test   = model.Test()
engine = model.Engine()
telem  = model.Telem()

# Set model dependencies
engine.add_deps([test])

# Initialize state from top-level model
telem.init(outputPath.as_posix(), "# test_engine.py", 3)
engine.init_state(telem)

# Initialize models
stateFields = ["time"]
test.init(stateFields)

enginePath = inputPath / "AeroTech_J450DM.eng"
timeData, thrustData, massData = preproc_engine.load(enginePath)
engine.init(timeData, thrustData, massData)

# Test model
time    = np.arange(0, 3, 0.01)
thrust  = np.empty(len(time))
massEng = np.empty(len(time))

for iTime in range(len(time)):

    test.set_state_data("time", time[iTime])
    engine.update()
    telem.update()

    thrust[iTime]  = test.get_state_data("thrust")
    massEng[iTime] = test.get_state_data("massEng")


# Export data to *.mat
telem.finalize()
postproc_flight.npy_to_mat(outputPath / "telem.npy")

# Visualization
plt.rcParams['text.usetex'] = True

fig, ax = plt.subplots()
ax.plot(time, thrust)
ax.set_title("Thrust Profile")
ax.set_xlabel("Time [s]")
ax.set_ylabel("Thrust [N]")
fig.savefig(outputPath / "thrust.png", bbox_inches="tight")

fig, ax = plt.subplots()
ax.plot(time, massEng)
ax.set_title("Engine Mass Profile")
ax.set_xlabel("Time [s]")
ax.set_ylabel("Mass [kg]")
fig.savefig(outputPath / "mass.png", bbox_inches="tight")
