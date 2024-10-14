#!/usr/bin/env python3

# System modules
import os
import sys
import argparse
import pathlib
import multiprocessing as mp
import colorama

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

    # Parse CLI arguments
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--input", type=str, help="Input file")
    parser.add_argument("-o", "--output", type=str, help="Output directory")

    args = parser.parse_args()

    # Run program

    if args.input is None:

        # Run GUI
        gui_main.exec()

    else:

        # Run CLI (headless)
        cli_intro()
        inputPath = pathlib.Path(args.input)

        if args.output is None:

            outputPath = pathlib.Path("output")
            print(colorama.Fore.RED, end='')
            print(f"No output directory given, using: {outputPath.resolve()}")
            print(colorama.Style.RESET_ALL)

            if not os.path.exists(outputPath):
                os.mkdir(outputPath)

        else:
            outputPath = pathlib.Path(args.output)

        outputPath = outputPath / inputPath.stem # Add subdirectory
        print(f"Reading input file: {colorama.Fore.YELLOW}{inputPath.resolve()}{colorama.Style.RESET_ALL}")
        inputParams = util_yaml.load(inputPath)

        exec.run(inputParams, outputPath)
