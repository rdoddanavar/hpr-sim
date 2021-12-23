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
configPathRel = "../../config"
configInput   = None
configOutput  = None
inputDict     = None
outputPath2   = None

#------------------------------------------------------------------------------#

def exec(inputPath, outputPath):

    """
    Executes simluation using paramters defined in input file

    :param inputPath: File path to \*.yml input file
    :type inputPath: str

    :param outputPath: File path for simulation data output
    :type outputPath: str
    """

    # Pre-processing
    util_unit.config()
    
    configPath  = pathlib.Path(__file__).parent / configPathRel
    configPath  = configPath.resolve()

    global configInput
    configInput = util_yaml.load(str(configPath / "config_input.yml"))

    global configOutput
    configOutput = util_yaml.load(str(configPath / "config_output.yml"))

    global inputDict
    inputDict = util_yaml.load(inputPath)
    util_yaml.process(inputDict)                  # Validate raw input file, resolve references
    preproc_input.process(inputDict, configInput) # Validate input parameter values
    exec_rand.check_dist(inputDict)               # Validate random distribution choice, parameters

    # Output setup
    global outputPath2
    inputName   = pathlib.Path(inputPath).stem
    outputPath2 = pathlib.Path(outputPath) / inputName
    
    if not os.path.exists(outputPath2):
        os.mkdir(outputPath2)

    # Validate telemetry output fields
    telemInvalid = set(configOutput["telem"]) - set(model.Flight.telemFieldsDefault)

    if telemInvalid:
        raise ValueError("Invalid telemetry fields", telemInvalid)
    else:
        model.Flight.set_telem(configOutput["telem"])

    # TODO: if issues with config_ouput.yml, resort to default fields
    # Also, populate output stats fields

    # Sim execution
    mode    = inputDict["exec"]["mode"]["value"]
    numProc = inputDict["exec"]["numProc"]["value"]
    numMC   = inputDict["exec"]["numMC"]["value"]
    
    if mode == "nominal":
        run_flight(inputDict, 0)

    elif mode == "montecarlo":
    
        pool  = mp.Pool(numProc)
        iRuns = range(numMC)

        # could probably use functools.partial here to avoid global inputDict
        pool.map_async(run_flight_mc, iRuns)

        pool.close()
        pool.join()
    
    # Post-processing

#------------------------------------------------------------------------------#

def run_flight_mc(iRun):

    inputDictRun = copy.deepcopy(inputDict)

    seedMaster = inputDict["exec"]["seed"]["value"]
    seedRun    = seedMaster + iRun
    
    inputDictRun["exec"]["seed"]["value"] = seedRun

    exec_rand.mc_draw(inputDictRun, configInput)
    run_flight(inputDictRun, iRun)

#------------------------------------------------------------------------------#

def run_flight(inputDictRun, iRun):

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
    #write_summary()

#------------------------------------------------------------------------------#

def write_output(iRun, inputDictRun, flight):

    # Setup run output folder
    outputPath3 = outputPath2 / f"run{iRun}"
    
    if not os.path.exists(outputPath3):
        os.mkdir(outputPath3)
    
    # Write input *.yml
    # Archives montecarlo draw for run recreation
    inputDictRun["exec"]["mode"]["value"] = "nominal"

    for group in inputDictRun.keys():
        for param in inputDictRun[group].keys():

            props = inputDictRun[group][param].keys()

        if "unit" in props:
            
            value    = inputDictRun[group][param]["value"]
            quantity = configInput[group][param]["quantity"]
            unit     = inputDictRun[group][param]["unit"]

            # Convert values back to original units specified by user

            if quantity:
                value = util_unit.convert(value, quantity, "default", unit)
                inputDictRun[group][param]["value"] = value

    outputYml = outputPath3 / "input.yml"

    with open(str(outputYml), 'w') as file:
        yaml.dump(inputDictRun, file, sort_keys=False, indent=4)

    # Write telemetry *.csv
    outputCsv = outputPath3 / "telem.csv"
    flight.write_telem(str(outputCsv))

    # Write statistics *.txt
    outputTxt = outputPath3 / "stats.txt"
    flight.write_stats(str(outputTxt))

#------------------------------------------------------------------------------#

# def write_summary():

#------------------------------------------------------------------------------#

if __name__ == "__main__":

    inputPath  = sys.argv[1]
    outputPath = sys.argv[2]

    exec(inputPath, outputPath)