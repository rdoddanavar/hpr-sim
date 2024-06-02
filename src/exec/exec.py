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
import tqdm
import colorama

# Path modifications
paths = ["../../build/src", "../preproc", "../util"]

for item in paths:
    addPath = pathlib.Path(__file__).parent / item
    sys.path.append(str(addPath.resolve()))

# Project modules
import exec_rand
import util_yaml
import util_unit
import util_misc
import preproc_input
import preproc_engine
import preproc_aerodynamics

compilerPath       = util_misc.get_cmake_cache("../../build/CMakeCache.txt", "CMAKE_CXX_COMPILER")
compilerPathParent = str(pathlib.Path(compilerPath).parent)

if os.name == "nt":
    # Explicitly add path to libstdc++
    os.add_dll_directory(compilerPathParent)

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

def cli_intro():

    version = util_misc.get_cmake_cache("../../build/CMakeCache.txt", "CMAKE_PROJECT_VERSION")

    colorama.init()

    print(f"{colorama.Fore.CYAN}hpr-sim v{version}")
    print(f"https://github.com/rdoddanavar/hpr-sim{colorama.Style.RESET_ALL}")
    print()

#------------------------------------------------------------------------------#

def exec(inputPath, outputPath):

    """
    Executes simluation using paramters defined in input file

    :param inputPath: File path to \*.yml input file
    :type inputPath: str

    :param outputPath: File path for simulation data output
    :type outputPath: str
    """

    cli_intro()

    # Pre-processing
    util_unit.config()

    configPath  = pathlib.Path(__file__).parent / "../../config" #configPathRel
    configPath  = configPath.resolve()

    configInput = util_yaml.load(str(configPath / "config_input.yml"))
    util_yaml.process(configInput)

    configOutput = util_yaml.load(str(configPath / "config_output.yml"))

    print(f"Reading input file: {colorama.Fore.YELLOW}{inputPath}{colorama.Style.RESET_ALL}")
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
    print(f"Reading propulsion data: {colorama.Fore.YELLOW}{enginePath}{colorama.Style.RESET_ALL}")
    timeEng, thrustEng, massEng = preproc_engine.load(enginePath)

    # Aeromodel data
    aeroPath = simInput["aerodynamics"]["inputPath"]["value"]
    print(f"Reading aerodynamic data: {colorama.Fore.YELLOW}{aeroPath}{colorama.Style.RESET_ALL}")
    (machData, alphaData, aeroData) = preproc_aerodynamics.load_csv(aeroPath)

    # Collect all sim data
    simData = SimData(machData, alphaData, aeroData, timeEng, thrustEng, massEng)

    # Sim execution
    mode    = simInput["exec"]["mode"]["value"]
    numProc = simInput["exec"]["numProc"]["value"]
    numMC   = simInput["exec"]["numMC"]["value"]

    print()
    print(f"Simulation output available at: {colorama.Fore.YELLOW}{simConfig.outputPath2}/run*{colorama.Style.RESET_ALL}")

    if mode == "nominal":

        print("Executing nominal run")
        run_sim(simInput, simConfig, simData, 0)

    elif mode == "montecarlo":

        print(f"Monte Carlo summary available at: {colorama.Fore.YELLOW}{simConfig.outputPath2}/summary.yml{colorama.Style.RESET_ALL}")
        print()

        with mp.Pool(numProc) as pool:

            print("Executing Monte Carlo runs:")

            # Setup progress bar
            pBar = tqdm.tqdm(total=numMC, unit="run", dynamic_ncols=True, colour="green")
            callback_fun = functools.partial(cli_status, pBar)

            # Execute parallel runs
            for iRun in range(numMC):
                pool.apply_async(run_sim_mc, (simInput, simConfig, simData, iRun), callback=callback_fun)

            # Pool cleanup
            pool.close()
            pool.join()

        # Write summary *.yml
        write_summary(simConfig.outputPath2 / "summary.yml", simInput["flight"]["precision"]["value"])

#------------------------------------------------------------------------------#

def cli_status(pBar, result):
    pBar.update()

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

    outputInput = outputPath3 / "input.yml"

    with open(str(outputInput), 'w') as file:
        yaml.dump(simInput, file, sort_keys=False, indent=4)

    # Write telemetry *.csv
    outputTelem = outputPath3 / "telem.csv"
    flight.write_telem(str(outputTelem))

    # Write statistics *.yml
    outputStats = outputPath3 / "stats.yml"
    flight.write_stats(str(outputStats))

#------------------------------------------------------------------------------#

def write_summary(filePathOut, nPrec):

    dir = pathlib.Path(filePathOut).parent
    subdirs = [subdir for subdir in dir.iterdir() if subdir.is_dir()]
    nSubdir = len(subdirs)

    first = True

    for iDir, subdir in enumerate(subdirs):

        filePathIn = subdir / "stats.yml"

        with open(filePathIn, 'r', encoding="utf8") as stream:
            stats = yaml.safe_load(stream)

        keys = list(stats.keys())

        if first:

            first   = False
            data    = copy.deepcopy(stats)
            summary = copy.deepcopy(stats)

            for key in keys:
                data[key]["Min"] = np.zeros(nSubdir)
                data[key]["Max"] = np.zeros(nSubdir)

        for key in keys:
            data[key]["Min"][iDir] = stats[key]["Min"]
            data[key]["Max"][iDir] = stats[key]["Max"]

    for key in keys:

        summaryMin = {}

        summaryMin["Min"]  = float(data[key]["Min"].min().round(decimals=nPrec))
        summaryMin["Max"]  = float(data[key]["Min"].max().round(decimals=nPrec))
        summaryMin["Mean"] = float(data[key]["Min"].mean().round(decimals=nPrec))
        summaryMin["Std"]  = float(data[key]["Min"].std().round(decimals=nPrec))

        summary[key]["Min"] = summaryMin

        summaryMax = {}

        summaryMax["Min"]  = float(data[key]["Max"].min().round(decimals=nPrec))
        summaryMax["Max"]  = float(data[key]["Max"].max().round(decimals=nPrec))
        summaryMax["Mean"] = float(data[key]["Max"].mean().round(decimals=nPrec))
        summaryMax["Std"]  = float(data[key]["Max"].std().round(decimals=nPrec))

        summary[key]["Max"] = summaryMax

    with open(filePathOut, 'w', encoding="utf8") as stream:
        yaml.dump(summary, stream, indent=4, explicit_start=True, explicit_end=True, sort_keys=False)

#------------------------------------------------------------------------------#

if __name__ == "__main__":

    inputPath  = sys.argv[1]
    outputPath = sys.argv[2]

    # TODO: mp.freeze_support() # Need this for pyinstaller

    exec(inputPath, outputPath)
