#!/usr/bin/env python3

# System modules
import sys
import argparse
import pathlib
import multiprocessing as mp
import colorama
from datetime import datetime

# Path modifications
paths = ["build/src", "src/exec", "src/gui", "src/preproc", "src/postproc", "src/util"]

for item in paths:
    addPath = pathlib.Path(__file__).parent / item
    sys.path.append(addPath.resolve().as_posix())

# Project modules
import exec
import gui_main
import postproc_flight
import util_yaml
import util_misc

#------------------------------------------------------------------------------#

if __name__ == "__main__":

    # Multiprocessing support for PyInstaller
    mp.freeze_support()

    # Parse CLI
    parser = argparse.ArgumentParser()
    parser.add_argument("--headless", action="store_true", help="Headless mode (no GUI)")
    parser.add_argument("-i", "--input", type=str, help="Input file path")
    parser.add_argument("output", type=str, help="Output file path")

    args       = parser.parse_args()
    inputPath  = pathlib.Path(args.input)
    outputPath = pathlib.Path(args.output)

    # CLI intro
    colorama.init()

    timeStamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    version   = util_misc.get_cmake_cache("CMAKE_PROJECT_VERSION")

    print(f"{colorama.Fore.CYAN}")
    print(f"{timeStamp}")
    print(f"hpr-sim v{version}")
    print(colorama.Style.RESET_ALL)

    # Run program

    if not(args.headless):
        gui_main.exec()
    elif inputPath is not None:

        print(f"Reading input file: {colorama.Fore.YELLOW}{inputPath.resolve()}{colorama.Style.RESET_ALL}")

        inputParams = util_yaml.load(inputPath)
        inputParams["meta"] = {"timeStamp": timeStamp, "version": version, "inputPath": str(inputPath.resolve())}

        exec.run(inputParams, outputPath)

    else:
        postproc_flight.postproc(outputPath)
