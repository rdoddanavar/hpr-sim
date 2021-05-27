# System modules
import sys # System utilities
import pdb # Python debugger
import pathlib
import multiprocessing as mp

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

#------------------------------------------------------------------------------#

def exec(inputPath, outputPath):

    # Pre-processing
    util_unit.config()

    configPath = pathlib.Path(__file__).parent / configPathRel
    configPath = str(configPath.resolve())
    configDict = util_yaml.load(configPath)

    inputDict = util_yaml.load(inputPath)
    util_yaml.process(inputDict)
    preproc_input.process(inputDict, configDict)

    # Initialize model - engine
    enginePath = inputDict["engine"]["inputPath"]["value"]
    timeEng, thrustEng, massEng = preproc_engine.load(enginePath)

    engine = model.Engine()
    engine.init(timeEng, thrustEng, massEng)

    # Initialize model - mass
    mass     = model.Mass()
    massBody = inputDict["mass"]["massBody"]["value"]

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
    dt = 0.01
    tf = 50.0

    flight.init(t0, dt, tf)

    # Sim execution
    run_mc(flight, outputPath, 1)

    # Post-processing

#------------------------------------------------------------------------------#

outputName = "output"

def run_mc(flight, outputPath, iRun):

    flight.update()
    outputPath += f"{outputName}{iRun}.csv"
    flight.write_telem(outputPath)

#------------------------------------------------------------------------------#

if __name__ == "__main__":

    inputPath  = sys.argv[1]
    outputPath = sys.argv[2]

    exec(inputPath, outputPath)