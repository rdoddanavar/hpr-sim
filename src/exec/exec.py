# System modules
import os
import shutil
import pathlib
import copy
import multiprocessing as mp
import numpy as np
import yaml
import functools
import tqdm
import colorama

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
import telem as telemetry

#------------------------------------------------------------------------------#

def run(inputParams: dict, outputPath: pathlib.Path) -> None:

    """
    Executes simluation using input parameters and outputs artifacts to specified directory.

    :param inputParams: Input parameter dictionary
    :type inputParams: dict

    :param outputPath: File path for simulation data output
    :type outputPath: pathlib.Path
    """

    # Pre-processing
    util_yaml.process(inputParams)     # Validate raw input file, resolve references
    preproc_input.process(inputParams) # Validate input parameter values
    exec_rand.check_dist(inputParams)  # Validate random distribution choice, parameters

    # Output setup

    if os.path.exists(outputPath):
        shutil.rmtree(outputPath)

    os.mkdir(outputPath)

    # Motor data
    enginePath = pathlib.Path(inputParams["engine"]["inputPath"]["value"])
    print(f"Reading propulsion data: {colorama.Fore.YELLOW}{enginePath.resolve()}{colorama.Style.RESET_ALL}")
    timeEng, thrustEng, massEng = preproc_engine.load(enginePath)

    # Aeromodel data
    aeroPath = pathlib.Path(inputParams["aerodynamics"]["inputPath"]["value"])
    print(f"Reading aerodynamic data: {colorama.Fore.YELLOW}{aeroPath.resolve()}{colorama.Style.RESET_ALL}")
    (machData, alphaData, aeroData) = preproc_aerodynamics.load_csv(aeroPath)

    # Collect all sim data
    modelData = {}

    # TODO: Refactor this into "independent" and "dependent" lookup data
    modelData["engine"] = {"time": timeEng, "thrust": thrustEng, "mass" : massEng}

    # TODO: Refactor this into "independent" and "dependent" lookup data
    modelData["aerodynamics"] = {"mach": machData, "alpha": alphaData, "aero": aeroData }

    # Sim execution
    mcMode   = inputParams["exec"]["mcMode"]["value"]
    numMC    = inputParams["exec"]["numMC"]["value"]
    procMode = inputParams["exec"]["procMode"]["value"]
    numProc  = inputParams["exec"]["numProc"]["value"]

    print()
    print(f"Simulation output available at: {colorama.Fore.YELLOW}{outputPath.resolve()}{colorama.Style.RESET_ALL}")

    if mcMode == "nominal":

        print("Executing nominal run")
        run_sim(inputParams, outputPath, modelData, iRun=0)

    elif mcMode == "montecarlo":

        summaryPath = outputPath / "summary.yml"
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
                run_sim_mc(inputParams, outputPath, modelData, iRun)
                callback_serial()

        elif procMode == "parallel":

            with mp.Pool(numProc) as pool:

                # Execute parallel runs
                for iRun in range(numMC):
                    pool.apply_async(run_sim_mc, (inputParams, outputPath, modelData, iRun), callback=callback_parallel)

                # Pool cleanup
                pool.close()
                pool.join()

        # Write summary *.yml
        write_mc_summary(inputParams, outputPath)

#------------------------------------------------------------------------------#

def cli_status(pBar, result):
    pBar.update()

#------------------------------------------------------------------------------#

def run_sim_mc(inputParams, outputPath, modelData, iRun):

    inputParamsMC = copy.deepcopy(inputParams)

    seedMaster = inputParams["exec"]["seed"]["value"]
    seedRun    = seedMaster + iRun

    inputParamsMC["exec"]["seed"]["value"] = seedRun

    exec_rand.mc_draw(inputParamsMC)

    run_sim(inputParamsMC, outputPath, modelData, iRun)

#------------------------------------------------------------------------------#

def run_sim(inputParams, outputPath, modelData, iRun):

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

    # Setup telemetry
    numMC     = inputParams["exec"]["numMC"]["value"]
    telemMode = inputParams["exec"]["telemMode"]["value"]
    telemPrec = inputParams["exec"]["telemPrec"]["value"]

    # Setup run output folder
    outputPath = outputPath / f"run{iRun+1}"
    os.mkdir(outputPath)

    # Echo input file for specific run
    write_input(inputParams, outputPath, iRun)

    # Make header string for metadata
    metaStr0 = f"# {util_misc.get_timestamp()}\n"
    metaStr1 = f"# hpr-sim v{util_misc.get_version()}\n"
    metaStr2 = f"# Run: {iRun+1}/{numMC}"
    metaStr  = metaStr0 + metaStr1 + metaStr2

    telem = telemetry.Telem(telemMode, telemPrec, outputPath.resolve().as_posix(), metaStr)

    # Initialize state from top-level model
    flight.init_state(telem)
    
    # Initialize models

    engine.init(modelData["engine"]["time"]  ,
                modelData["engine"]["thrust"],
                modelData["engine"]["mass"]  )

    massBody = inputParams["mass"]["massBody"]["value"]
    mass.init(massBody)

    latitude = inputParams["geodetic"]["latitude"]["value"]
    altitude = inputParams["geodetic"]["altitude"]["value"]
    geodetic.init(latitude, altitude)

    temperature = inputParams["atmosphere"]["temperature"]["value"]
    pressure    = inputParams["atmosphere"]["pressure"]["value"]
    atmosphere.init(temperature, pressure)

    refArea = inputParams["aerodynamics"]["refArea"]["value"]
    aerodynamics.init(refArea, 
                      modelData["aerodynamics"]["mach"]              ,
                      modelData["aerodynamics"]["alpha"]             ,
                      modelData["aerodynamics"]["aero"]["cpTotal"]   ,
                      modelData["aerodynamics"]["aero"]["clPowerOff"],
                      modelData["aerodynamics"]["aero"]["cdPowerOff"],
                      modelData["aerodynamics"]["aero"]["clPowerOn"] ,
                      modelData["aerodynamics"]["aero"]["cdPowerOn"] )

    eom.init()

    solverMethod = inputParams["flight"]["solverMethod"]["value"]
    solverStep   = inputParams["flight"]["solverStep"]["value"]
    flight.init(solverMethod, solverStep)

    # Execute flight
    flight.update()

#------------------------------------------------------------------------------#

def write_input(inputParams, outputPath, iRun):

    # Write input *.yml
    # Archives montecarlo draw for run recreation
    inputParamsEcho = copy.deepcopy(inputParams)

    # Settings for individual run recreation
    inputParamsEcho["exec"]["mcMode"]["value"]   = "nominal"
    inputParamsEcho["exec"]["numMC"]["value"]    = 1
    inputParamsEcho["exec"]["procMode"]["value"] = "serial"
    inputParamsEcho["exec"]["numProc"]["value"]  = 1

    for group in inputParamsEcho.keys():
        for param in inputParamsEcho[group].keys():

            props = inputParamsEcho[group][param].keys()

            if "unit" in props:

                value    = inputParamsEcho[group][param]["value"]
                quantity = preproc_input.configInput[group][param]["quantity"]
                unit     = inputParamsEcho[group][param]["unit"]

                # Convert values back to original units specified by user

                if quantity:
                    value = util_unit.convert(value, quantity, "default", unit)
                    inputParamsEcho[group][param]["value"] = value

    filePath = outputPath / "input.yml"

    with open(filePath, 'w') as file:

        # Get MC count from original parameter set
        numMC = inputParams["exec"]["numMC"]["value"]

        # Make header string for metadata
        metaStr0 = f"# {util_misc.get_timestamp()}\n"
        metaStr1 = f"# hpr-sim v{util_misc.get_version()}\n"
        metaStr2 = f"# Run: {iRun+1}/{numMC}"
        metaStr  = metaStr0 + metaStr1 + metaStr2

        file.write(f"{metaStr}\n")

        yaml.dump(inputParamsEcho, file, indent=4, explicit_start=True, explicit_end=True, sort_keys=False)

#------------------------------------------------------------------------------#

def write_mc_summary(inputParams, outputPath):

    filePath = outputPath / "summary.yml"
    dir      = filePath.parent
    subdirs  = [subdir for subdir in dir.iterdir() if subdir.is_dir()]
    nSubdir  = len(subdirs)
    nPrec    = inputParams["exec"]["telemPrec"]["value"]
    numMC    = inputParams["exec"]["numMC"]["value"]

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
        metaStr0 = f"# {util_misc.get_timestamp()}\n"
        metaStr1 = f"# hpr-sim v{util_misc.get_version()}\n"
        metaStr2 = f"# Run: {numMC}/{numMC}"
        metaStr  = metaStr0 + metaStr1 + metaStr2

        stream.write(f"{metaStr}\n")

        yaml.dump(summary, stream, indent=4, explicit_start=True, explicit_end=True, sort_keys=False)
