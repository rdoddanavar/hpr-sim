# System modules
import sys # System utilities
import pdb # Python debugger
from pathlib import Path
import numpy as np
import matplotlib.pyplot as plt

# Path modifications
paths = ["../build/src", "../src/preproc", "../src/util"]

for item in paths:
    addPath = Path(__file__).parent / item
    sys.path.append(str(addPath.resolve()))

#-------------------------------------------------------#

import preproc_engine
import model

time, thrust, mass = preproc_engine.load("../input/prop/AeroTech_J450DM.eng")

eng = model.Engine()
eng.initialize(time, thrust, mass)

timeEval = np.linspace(0, time[-1], int(1e3))
thrustEval = np.array([])

#pdb.set_trace()

for val in timeEval:
    
    eng.update(val)
    thrustEval = np.append(thrustEval, eng.state["thrust"])

plt.plot(time, thrust, label="Data", marker='o')
plt.plot(timeEval, thrustEval, label="Interpolated")

plt.xlabel("Time [sec]")
plt.ylabel("Thrust [N]")
plt.grid()
plt.legend()
plt.show()

pdb.set_trace()