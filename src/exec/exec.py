# System modules
import sys
import os
import pdb
import pathlib
import multiprocessing as mp
import functools

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
outputPath    = None
inputDict     = None

#------------------------------------------------------------------------------#

def exec(inputPath, outputPathBase):

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
    outputDir = pathlib.Path(inputPath).stem

    if not os.path.exists(outputDir):
        os.mkdir(outputDir)

    # Sim execution
    numProc = inputDict['exec']['numProc']['value']
    numMC   = inputDict['exec']['numMC']['value']

    pool  = mp.Pool(numProc)
    iRuns = range(numMC)

    pool.map_async(run_mc, iRuns)

    pool.close()
    pool.join()
    
    # Post-processing

#------------------------------------------------------------------------------#

def run_mc(iRun):

    seedMaster = inputDict['exec']['seedMaster']['value']
    seedRun    = seedMaster + iRun

    # Initialize model - engine
    enginePath = inputDict["engine"]["inputPath"]["value"]
    timeEng, thrustEng, massEng = preproc_engine.load(enginePath)

    engine = model.Engine()
    engine.init(timeEng, thrustEng, massEng)

    # Initialize model - mass
    mass     = model.Mass()
    massBody = inputDict["mass"]["massBody"]["value"]

    #--------------------#
    massBody += iRun
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
    dt = inputDict['exec']['timeStep']['value']
    tf = inputDict['exec']['timeFlight']['value']

    flight.init(t0, dt, tf)
    flight.update()

    # Write output
    outputPath = f"output/output{iRun}.csv"
    flight.write_telem(outputPath)

#------------------------------------------------------------------------------#

if __name__ == "__main__":

    inputPath  = sys.argv[1]
    outputPath = sys.argv[2]

    exec(inputPath, outputPath)