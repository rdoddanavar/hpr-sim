# System modules
import os
import shutil
import pathlib
import copy
import multiprocessing as mp
import numpy as np
import yaml
from dataclasses import dataclass
import functools
import tqdm
import colorama
from datetime import datetime

# Project modules (python)
import exec_rand
import util_yaml
import util_unit
import util_misc
import preproc_input
import preproc_engine
import preproc_aerodynamics

# Project modules (pybind11)
util_misc.pybind11_setup()
import model

#------------------------------------------------------------------------------#

@dataclass
class Metadata:
    timeStamp: str
    version: str

@dataclass
class SimConfig:
    input: dict
    output: dict
    inputPath: pathlib.Path
    outputPath2: pathlib.Path
    metadata: str

@dataclass
class SimData:
    machData: dict
    alphaData: dict
    aeroData: dict
    timeEng: np.ndarray
    thrustEng: np.ndarray
    massEng: np.ndarray

#------------------------------------------------------------------------------#

def cli_intro(metadata):

    colorama.init()

    print(f"{colorama.Fore.CYAN}")
    print(f"{metadata.timeStamp}")
    print(f"hpr-sim v{metadata.version}")
    print(colorama.Style.RESET_ALL)

#------------------------------------------------------------------------------#

def run(inputPath, outputPath, configPath):

    """
    Executes simluation using paramters defined in input file

    :param inputPath: File path to \*.yml input file
    :type inputPath: str

    :param outputPath: File path for simulation data output
    :type outputPath: str
    """

    # Metadata
    timeStamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    version   = util_misc.get_cmake_cache("CMAKE_PROJECT_VERSION")
    metadata  = Metadata(timeStamp, version)

    cli_intro(metadata)

    # Pre-processing
    configInput = util_yaml.load(configPath / "config_input.yml")
    util_yaml.process(configInput)

    print(f"Reading input file: {colorama.Fore.YELLOW}{inputPath.resolve()}{colorama.Style.RESET_ALL}")
    simInput = util_yaml.load(inputPath)
    util_yaml.process(simInput)                  # Validate raw input file, resolve references
    preproc_input.process(simInput, configInput) # Validate input parameter values
    exec_rand.check_dist(simInput)               # Validate random distribution choice, parameters

    # Output setup
    outputPath2 = outputPath / inputPath.stem

    if os.path.exists(outputPath2):
        shutil.rmtree(outputPath2)

    os.mkdir(outputPath2)

    # Validate telemetry output fields
    configOutput = util_yaml.load(configPath / "config_output.yml")
    telemInvalid = set(configOutput["telem"]) - set(model.Flight.telemFieldsDefault)

    configOutput["telemUnits"] = []

    if telemInvalid:
        raise ValueError("Invalid telemetry fields", telemInvalid)
    else:
        for i in range(len(model.Flight.telemFieldsDefault)):
            if configOutput["telem"][i] in model.Flight.telemFieldsDefault:
                configOutput["telemUnits"].append(model.Flight.telemUnitsDefault[i])

    simConfig = SimConfig(configInput, configOutput, inputPath, outputPath2, metadata)

    # TODO: if issues with config_ouput.yml, resort to default fields
    # Also, populate output stats fields

    # Motor data
    enginePath = pathlib.Path(simInput["engine"]["inputPath"]["value"])
    print(f"Reading propulsion data: {colorama.Fore.YELLOW}{enginePath.resolve()}{colorama.Style.RESET_ALL}")
    timeEng, thrustEng, massEng = preproc_engine.load(enginePath)

    # Aeromodel data
    aeroPath = pathlib.Path(simInput["aerodynamics"]["inputPath"]["value"])
    print(f"Reading aerodynamic data: {colorama.Fore.YELLOW}{aeroPath.resolve()}{colorama.Style.RESET_ALL}")
    (machData, alphaData, aeroData) = preproc_aerodynamics.load_csv(aeroPath)

    # Collect all sim data
    simData = SimData(machData, alphaData, aeroData, timeEng, thrustEng, massEng)

    # Sim execution
    mcMode   = simInput["exec"]["mcMode"]["value"]
    numMC    = simInput["exec"]["numMC"]["value"]
    procMode = simInput["exec"]["procMode"]["value"]
    numProc  = simInput["exec"]["numProc"]["value"]

    print()
    print(f"Simulation output available at: {colorama.Fore.YELLOW}{simConfig.outputPath2.resolve()}{colorama.Style.RESET_ALL}")

    if mcMode == "nominal":

        print("Executing nominal run")
        run_sim(simInput, simConfig, simData, 0)

    elif mcMode == "montecarlo":

        summaryPath = simConfig.outputPath2 / "summary.yml"
        print(f"Monte Carlo summary available at: {colorama.Fore.YELLOW}{summaryPath.resolve()}{colorama.Style.RESET_ALL}")
        print()
        print("Executing Monte Carlo runs:")

        # Setup progress bar
        pBar = tqdm.tqdm(total=numMC, unit="run", dynamic_ncols=True, colour="green")

        # Callback functions to update progress bar
        callback_parallel = functools.partial(cli_status, pBar)
        callback_serial   = functools.partial(callback_parallel, None)

        if procMode == "serial":

            for iRun in range(numMC):
                run_sim_mc(simInput, simConfig, simData, iRun)
                callback_serial()

        elif procMode == "parallel":

            with mp.Pool(numProc) as pool:

                # Execute parallel runs
                for iRun in range(numMC):
                    pool.apply_async(run_sim_mc, (simInput, simConfig, simData, iRun), callback=callback_parallel)

                # Pool cleanup
                pool.close()
                pool.join()

        # Write summary *.yml
        write_mc_summary(simConfig, simInput)

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

    telemMode = simInput["flight"]["telemMode"]["value"]
    nPrec     = simInput["flight"]["precision"]["value"]
    numMC     = simInput["exec"]["numMC"]["value"]

    # Setup run output folder
    outputPath3 = simConfig.outputPath2 / f"run{iRun}"
    os.mkdir(outputPath3)

    # Make header string for metadata
    metaStr0 = f"# {simConfig.metadata.timeStamp}\n"
    metaStr1 = f"# hpr-sim v{simConfig.metadata.version}\n"
    metaStr2 = f"# Input: {simConfig.inputPath}\n"
    metaStr3 = f"# Run: {iRun}/{numMC}"
    metaStr  = metaStr0 + metaStr1 + metaStr2 + metaStr3

    flight.init(telemMode, nPrec, outputPath3.resolve().as_posix(), metaStr)

    # Execute flight
    flight.update()
    write_input(simInput, simConfig, iRun)

#------------------------------------------------------------------------------#

def write_input(simInput, simConfig, iRun):

    # Write input *.yml
    # Archives montecarlo draw for run recreation
    simInput["exec"]["mcMode"]["value"] = "nominal"

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

    outputPath3 = simConfig.outputPath2 / f"run{iRun}"
    outputInput = outputPath3 / "input.yml"

    with open(outputInput, 'w') as file:

        numMC = simInput["exec"]["numMC"]["value"]

        # Make header string for metadata
        metaStr0 = f"# {simConfig.metadata.timeStamp}\n"
        metaStr1 = f"# hpr-sim v{simConfig.metadata.version}\n"
        metaStr2 = f"# Input: {simConfig.inputPath}\n"
        metaStr3 = f"# Run: {iRun}/{numMC}"
        metaStr  = metaStr0 + metaStr1 + metaStr2 + metaStr3

        file.write(f"{metaStr}\n")

        yaml.dump(simInput, file, indent=4, explicit_start=True, explicit_end=True, sort_keys=False)

#------------------------------------------------------------------------------#

def write_mc_summary(simConfig, simInput):

    filePath = simConfig.outputPath2 / "summary.yml"
    dir      = filePath.parent
    subdirs  = [subdir for subdir in dir.iterdir() if subdir.is_dir()]
    nSubdir  = len(subdirs)
    nPrec    = simInput["flight"]["precision"]["value"]
    numMC    = simInput["exec"]["numMC"]["value"]

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

    with open(filePath, 'w', encoding="utf8") as stream:

        # Make header string for metadata
        metaStr0 = f"# {simConfig.metadata.timeStamp}\n"
        metaStr1 = f"# hpr-sim v{simConfig.metadata.version}\n"
        metaStr2 = f"# Input: {simConfig.inputPath}\n"
        metaStr3 = f"# Run: {numMC}/{numMC}"
        metaStr  = metaStr0 + metaStr1 + metaStr2 + metaStr3

        stream.write(f"{metaStr}\n")

        yaml.dump(summary, stream, indent=4, explicit_start=True, explicit_end=True, sort_keys=False)
