# System modules
import sys # System utilities
import pdb # Python debugger
from pathlib import Path
import numpy as np
import matplotlib.pyplot as plt
import math

# Path modifications
paths = ["../build/src", "../src/preproc", "../src/util"]

for item in paths:
    addPath = Path(__file__).parent / item
    sys.path.append(str(addPath.resolve()))

#-----------------------------------------------------------------------------#

import preproc_engine
import model

# Initialize engine model
time, thrust, mass = preproc_engine.load("../input/engine/AeroTech_J450DM.eng")

eng = model.Engine()
eng.init(time, thrust, mass)

# Initialize geodetic model
# SARA launch site: (32.269798483027344, -111.27475082401067)
lat  = 32.2698
lat *= math.pi/180

geo = model.Geodetic()
geo.init(lat)

# Initialize EOM model
eom = model.EOM()
eom.init()

eom.add_dep(eng)
eom.add_dep(geo)

#eom.test(0.0)

pdb.set_trace()