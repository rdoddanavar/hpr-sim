# System modules
import sys
import os
import pdb
import pathlib
import copy
import multiprocessing as mp
import numpy as np
import yaml

# Path modifications
paths = ["../../build/src", "../preproc", "../util"]

for item in paths:
    addPath = pathlib.Path(__file__).parent / item
    sys.path.append(str(addPath.resolve()))

# Project modules
import exec_rand
import util_yaml
import util_unit
import preproc_input
import preproc_engine
import model

#------------------------------------------------------------------------------#

# Module variables
configPathRel = "../../config/config_input.yml"
outputPath2   = None
inputDict     = None

#------------------------------------------------------------------------------#

def exec(inputPath, outputPath):

    # Pre-processing
    util_unit.config()

    configPath = pathlib.Path(__file__).parent / configPathRel
    configPath = str(configPath.resolve())
    configDict = util_yaml.load(configPath)

    global inputDict
    inputDict = util_yaml.load(inputPath)
    util_yaml.process(inputDict)
    preproc_input.process(inputDict, configDict)

    # Output setup
    global outputPath2
    inputName   = pathlib.Path(inputPath).stem
    outputPath2 = pathlib.Path(outputPath) / inputName
    
    if not os.path.exists(outputPath2):
        os.mkdir(outputPath2)

    # Sim execution
    mode    = inputDict["exec"]["mode"]["value"]
    numProc = inputDict["exec"]["numProc"]["value"]
    numMC   = inputDict["exec"]["numMC"]["value"]
    
    if mode == "nominal":
        
        #run_flight(np.nan)
        run_flight(0)

    elif mode == "montecarlo":
    
        pool  = mp.Pool(numProc)
        iRuns = range(numMC)

        pool.map_async(run_flight, iRuns)

        pool.close()
        pool.join()
    
    # Post-processing

#------------------------------------------------------------------------------#

def run_flight(iRun):

    inputDictRun = copy.deepcopy(inputDict)

    if not(np.isnan(iRun)):

        seedMaster = inputDict["exec"]["seed"]["value"]
        seedRun    = seedMaster + iRun
        
        inputDictRun["exec"]["seed"]["value"] = seedRun

        exec_rand.mc_draw(inputDictRun)

    # Initialize model - engine
    enginePath = inputDictRun["engine"]["inputPath"]["value"]
    timeEng, thrustEng, massEng = preproc_engine.load(enginePath)

    engine = model.Engine()
    engine.init(timeEng, thrustEng, massEng)

    # Initialize model - mass
    mass     = model.Mass()
    massBody = inputDictRun["mass"]["massBody"]["value"]

    mass.add_dep(engine)
    mass.init(massBody)

    # Initialize model - geodetic
    geodetic = model.Geodetic()
    latitude = inputDictRun["geodetic"]["latitude"]["value"]

    geodetic.init(latitude)

    # Initialize model - EOM
    eom = model.EOM()

    eom.add_dep(engine)
    eom.add_dep(mass)
    eom.add_dep(geodetic)
    eom.init()

    # Initialize model - flight
    flight = model.Flight()
    flight.add_dep(eom)

    t0 = 0.0
    dt = inputDictRun["flight"]["timeStep"]["value"]
    tf = inputDictRun["flight"]["timeFlight"]["value"]

    flight.init(t0, dt, tf)

    flight.update() # Execute flight
    write_output(iRun, inputDictRun, flight)

#------------------------------------------------------------------------------#

def write_output(iRun, inputDictRun, flight):

    # Setup run output folder
    outputPath3 = outputPath2 / f"run{iRun}"
    
    if not os.path.exists(outputPath3):
        os.mkdir(outputPath3)
    
    # Write telemetry *.csv
    outputCSV = outputPath3 / "telem.csv"
    flight.write_telem(str(outputCSV))

    # Write input *.yml
    # Archives montecarlo draw for run recreation
    inputDictRun["exec"]["mode"]["value"] = "nominal"

    outputYML = outputPath3 / "input.yml"

    with open(str(outputYML), 'w') as file:
        yaml.dump(inputDictRun, file)

#------------------------------------------------------------------------------#

if __name__ == "__main__":

    inputPath  = sys.argv[1]
    outputPath = sys.argv[2]

    exec(inputPath, outputPath)