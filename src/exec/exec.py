# System modules
import sys
import os
import pdb
import pathlib
import copy
import multiprocessing as mp
import numpy as np

# Path modifications
paths = ["../../build/src", "../preproc", "../util"]

for item in paths:
    addPath = pathlib.Path(__file__).parent / item
    sys.path.append(str(addPath.resolve()))

# Project modules
import util_yaml
import util_unit
import preproc_input
import preproc_engine
import model

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
    mode    = inputDict['exec']['mode']['value']
    numProc = inputDict['exec']['numProc']['value']
    numMC   = inputDict['exec']['numMC']['value']
    
    if mode == "nominal":
        
        #run_flight(None)
        run_flight(0)

    elif mode == "montecarlo":
    
        pool  = mp.Pool(numProc)
        iRuns = range(numMC)

        pool.map_async(run_flight, iRuns)

        pool.close()
        pool.join()
    
    # Post-processing

#------------------------------------------------------------------------------#

def mc_draw(rng):

    inputDictRun = copy.deepcopy(inputDict)

    for group in inputDictRun.keys():

        for param in inputDictRun[group].keys():

            props = inputDictRun[group][param].keys()

            if "dist" in props:

                distName  = inputDictRun[group][param]["name"]
                distParam = inputDictRun[group][param]["param"]
                rngFun    = getattr(rng, distName)
                rngEval   = rngFun(*distParam)

                breakpoint()

    breakpoint()

    return inputDictRun

#------------------------------------------------------------------------------#

# def run_mc
# def run_nominal

def run_flight(iRun):

    # Initialize RNG

    seedMaster = inputDict['exec']['seedMaster']['value']
    seedRun    = seedMaster + iRun
    philox     = np.random.Philox(seedRun)
    rng        = np.random.Generator(philox)

    inputDictRun = mc_draw(rng)

    breakpoint()

    # 

    # Initialize model - engine
    enginePath = inputDict["engine"]["inputPath"]["value"]
    timeEng, thrustEng, massEng = preproc_engine.load(enginePath)

    engine = model.Engine()
    engine.init(timeEng, thrustEng, massEng)

    # Initialize model - mass
    mass     = model.Mass()
    massBody = inputDict["mass"]["massBody"]["value"]

    #--------------------#
    #massBody += iRun
    #--------------------#

    mass.init(massBody)
    mass.add_dep(engine)

    # Initialize model - geodetic
    geodetic = model.Geodetic()
    latitude = inputDict["geodetic"]["latitude"]["value"]

    geodetic.init(latitude)

    # Initialize model - EOM
    eom = model.EOM()
    eom.init()

    eom.add_dep(engine)
    eom.add_dep(mass)
    eom.add_dep(geodetic)

    # Initialize model - flight
    flight = model.Flight()
    flight.add_dep(eom)

    t0 = 0.0
    dt = inputDict['flight']['timeStep']['value']
    tf = inputDict['flight']['timeFlight']['value']

    flight.init(t0, dt, tf)
    flight.update()

    # Write output
    outputPath3 = outputPath2 / f"run{iRun}"
    
    if not os.path.exists(outputPath3):
        os.mkdir(outputPath3)
    
    outputPath4 = outputPath3 / "telem.csv"
    flight.write_telem(str(outputPath4))

#------------------------------------------------------------------------------#

if __name__ == "__main__":

    inputPath  = sys.argv[1]
    outputPath = sys.argv[2]

    exec(inputPath, outputPath)