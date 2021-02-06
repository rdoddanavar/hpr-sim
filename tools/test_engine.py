# System modules
import sys # System utilities
import pdb # Python debugger
from pathlib import Path
import numpy as np

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

pdb.set_trace()