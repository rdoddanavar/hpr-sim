# System modules
import sys
import os
import pathlib
import copy
import multiprocessing as mp
import numpy as np
import yaml
from dataclasses import dataclass
import functools

# Path modifications
paths = ["../../build/src", "../preproc", "../util"]

for item in paths:
    addPath = pathlib.Path(__file__).parent / item
    sys.path.append(str(addPath.resolve()))

if os.name == "nt":
    os.add_dll_directory(r"C:\Users\roshan\AppData\Local\Programs\mingw64\bin")

# Project modules
import exec_rand
import util_yaml
import util_unit
import preproc_input
import preproc_engine
import preproc_aerodynamics
import model

#------------------------------------------------------------------------------#

# Module variables
# TODO: find alternatives to globals? Mutables like list or dict?
#configPathRel = "../../config"

# TODO: Replace this with a data class
@dataclass
class SimData:
    machData: dict
    alphaData: dict
    aeroData: dict
    timeEng: np.ndarray
    thrustEng: np.ndarray
    massEng: np.ndarray

@dataclass
class SimConfig:
    input: dict
    output: dict
    outputPath2: pathlib.Path

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

    configPath  = pathlib.Path(__file__).parent / "../../config" #configPathRel
    configPath  = configPath.resolve()

    configInput = util_yaml.load(str(configPath / "config_input.yml"))
    util_yaml.process(configInput)

    configOutput = util_yaml.load(str(configPath / "config_output.yml"))

    simInput = util_yaml.load(inputPath)
    util_yaml.process(simInput)                  # Validate raw input file, resolve references
    preproc_input.process(simInput, configInput) # Validate input parameter values
    exec_rand.check_dist(simInput)               # Validate random distribution choice, parameters

    # Output setup
    inputName   = pathlib.Path(inputPath).stem
    outputPath2 = pathlib.Path(outputPath) / inputName

    if not os.path.exists(outputPath2):
        os.mkdir(outputPath2)

    # Validate telemetry output fields
    telemInvalid = set(configOutput["telem"]) - set(model.Flight.telemFieldsDefault)

    configOutput["telemUnits"] = []

    if telemInvalid:
        raise ValueError("Invalid telemetry fields", telemInvalid)
    else:
        for i in range(len(model.Flight.telemFieldsDefault)):
            if configOutput["telem"][i] in model.Flight.telemFieldsDefault:
                configOutput["telemUnits"].append(model.Flight.telemUnitsDefault[i])

    # TEST
    simConfig = SimConfig(configInput, configOutput, outputPath2)
    # TEST
    
    # TODO: if issues with config_ouput.yml, resort to default fields
    # Also, populate output stats fields

    # Motor data
    enginePath = simInput["engine"]["inputPath"]["value"]
    timeEng, thrustEng, massEng = preproc_engine.load(enginePath)

    # Aeromodel data
    inputPath = simInput["aerodynamics"]["inputPath"]["value"]
    (machData, alphaData, aeroData) = preproc_aerodynamics.load_csv(inputPath)

    # Collect all sim data
    simData = SimData(machData, alphaData, aeroData, timeEng, thrustEng, massEng)

    # Sim execution
    mode    = simInput["exec"]["mode"]["value"]
    numProc = simInput["exec"]["numProc"]["value"]
    numMC   = simInput["exec"]["numMC"]["value"]

    if mode == "nominal":
        run_sim(simInput, simConfig, simData, 0)

    elif mode == "montecarlo":

        with mp.Pool(numProc) as pool:

            run_fun = functools.partial(run_sim_mc, simInput, simConfig, simData)
            iRuns   = range(numMC)

            pool.map_async(run_fun, iRuns)
            pool.close()
            pool.join()

        print("end!")

#------------------------------------------------------------------------------#

def run_sim_mc(simInput, simConfig, simData, iRun):

    simInputMC = copy.deepcopy(simInput)

    seedMaster = simInput["exec"]["seed"]["value"]
    seedRun    = seedMaster + iRun

    simInputMC["exec"]["seed"]["value"] = seedRun

    exec_rand.mc_draw(simInputMC, simConfig.input)

    run_sim(simInputMC, simConfig, simData, iRun)

#------------------------------------------------------------------------------#

def run_sim(simInput, simConfig, simData, iRun):

    # Create model instances
    engine       = model.Engine()
    mass         = model.Mass()
    geodetic     = model.Geodetic()
    atmosphere   = model.Atmosphere()
    aerodynamics = model.Aerodynamics()
    eom          = model.EOM()
    flight       = model.Flight()

    # Set model dependencies
    mass.add_deps([engine])
    atmosphere.add_deps([geodetic])
    aerodynamics.add_deps([engine, atmosphere])
    eom.add_deps([engine, mass, geodetic, aerodynamics])
    flight.add_deps([eom])

    # Initialize state from top-level model
    flight.init_state()

    # Initialize models

    engine.init(simData.timeEng, simData.thrustEng, simData.massEng)

    massBody = simInput["mass"]["massBody"]["value"]
    mass.init(massBody)

    latitude = simInput["geodetic"]["latitude"]["value"]
    altitude = simInput["geodetic"]["altitude"]["value"]
    geodetic.init(latitude, altitude)

    temperature = simInput["atmosphere"]["temperature"]["value"]
    pressure    = simInput["atmosphere"]["pressure"]["value"]
    atmosphere.init(temperature, pressure)

    refArea = simInput["aerodynamics"]["refArea"]["value"]
    aerodynamics.init(refArea, simData.machData, simData.alphaData, simData.aeroData["cpTotal"], simData.aeroData["clPowerOff"], simData.aeroData["cdPowerOff"], simData.aeroData["clPowerOn"], simData.aeroData["cdPowerOn"])

    eom.init()

    flight.set_telem(simConfig.output["telem"], simConfig.output["telemUnits"])

    t0    = 0.0
    dt    = simInput["flight"]["timeStep"]["value"]
    tf    = simInput["flight"]["timeFlight"]["value"]
    nPrec = simInput["flight"]["precision"]["value"]
    flight.init(t0, dt, tf, nPrec)

    # Execute flight
    flight.update()
    write_output(simInput, simConfig, iRun, flight)

#------------------------------------------------------------------------------#

def write_output(simInput, simConfig, iRun, flight):

    # Setup run output folder
    outputPath3 = simConfig.outputPath2 / f"run{iRun}"

    if not os.path.exists(outputPath3):
        os.mkdir(outputPath3)

    # Write input *.yml
    # Archives montecarlo draw for run recreation
    simInput["exec"]["mode"]["value"] = "nominal"

    for group in simInput.keys():
        for param in simInput[group].keys():

            props = simInput[group][param].keys()

            if "unit" in props:
                
                value    = simInput[group][param]["value"]
                quantity = simConfig.input[group][param]["quantity"]
                unit     = simInput[group][param]["unit"]

                # Convert values back to original units specified by user

                if quantity:
                    value = util_unit.convert(value, quantity, "default", unit)
                    simInput[group][param]["value"] = value

    outputYml = outputPath3 / "input.yml"

    with open(str(outputYml), 'w') as file:
        yaml.dump(simInput, file, sort_keys=False, indent=4)

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

    # TODO: mp.freeze_support() # Need this for pyinstaller

    exec(inputPath, outputPath)
