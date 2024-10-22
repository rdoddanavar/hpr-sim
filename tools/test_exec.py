#!/usr/bin/env python3

# System modules
import sys
import pathlib
import multiprocessing as mp
import colorama

# Path modifications
paths = ["build/src", "src/exec", "src/gui", "src/preproc", "src/postproc", "src/util"]

for item in paths:
    addPath = pathlib.Path(__file__).parent.parent / item
    sys.path.append(addPath.resolve().as_posix())

# Project modules
import exec
import gui_main
import postproc_flight
import util_yaml
import util_misc

#------------------------------------------------------------------------------#

def cli_intro():

    colorama.init()

    print(colorama.Fore.CYAN)
    print(f"{util_misc.get_timestamp()}")
    print(f"hpr-sim v{util_misc.get_version()}")
    print(colorama.Style.RESET_ALL)

#------------------------------------------------------------------------------#

if __name__ == "__main__":

    # Multiprocessing support for PyInstaller
    mp.freeze_support()

    inputPath  = pathlib.Path("input/unit_test.yml")
    outputPath = pathlib.Path("output")

    # Run program

    # Run CLI (headless)
    util_misc.set_timestamp()
    cli_intro()

    outputPath = outputPath / inputPath.stem # Add subdirectory
    print(f"Reading input file: {colorama.Fore.YELLOW}{inputPath.resolve()}{colorama.Style.RESET_ALL}")
    inputParams = util_yaml.load(inputPath)

    exec.run(inputParams, outputPath)
