# System modules
import sys
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
nProc = 4

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

    # # Sim execution
    #iRun = list(range(nProc))
    pool = mp.Pool(nProc)

    # pool.map_async(functools.partial(run_mc, flight), iRun)
    flights = [flight]*nProc
    # ##breakpoint()
    pool.map_async(test, flights)

    #pool.map_async(run_mc2, iRun)

    pool.close()
    pool.join()

    # #flight.update()
    # #flight.write_telem("output/output.csv")
    
    # Post-processing

#------------------------------------------------------------------------------#

outputPath = "output/"
outputName = "output"

def run_mc(flight, iRun):

    print("test")

    flight.update()
    #outputPath += f"{outputName}{iRun}.csv"
    outputPath = f"output/output{iRun}.csv"
    flight.write_telem(outputPath)

def run_mc2(iRun):

    print("test1")
    inputPath = "input/unit_test.yml"

    # Pre-processing
    util_unit.config()

    configPath = pathlib.Path(__file__).parent / configPathRel
    configPath = str(configPath.resolve())
    configDict = util_yaml.load(configPath)

    inputDict = util_yaml.load(inputPath)
    util_yaml.process(inputDict)
    preproc_input.process(inputDict, configDict)

    print("test2")

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
    dt = 0.01
    tf = 50.0

    flight.init(t0, dt, tf)

    flight.update()
    #outputPath += f"{outputName}{iRun}.csv"
    outputPath = f"output/output{iRun}.csv"
    flight.write_telem(outputPath)

def test(flight):
    print("done!")
    flight.update()
    print("done!")

def test2(num):
    print(str(num))

#------------------------------------------------------------------------------#

if __name__ == "__main__":

    inputPath  = sys.argv[1]
    outputPath = sys.argv[2]

    exec(inputPath, outputPath)