#!/usr/bin/env python3

# System modules
import sys
import argparse
import pathlib
import multiprocessing as mp

# Path modifications
paths = ["build/src", "src/exec", "src/gui", "src/preproc", "src/postproc", "src/util"]

for item in paths:
    addPath = pathlib.Path(__file__).parent / item
    sys.path.append(addPath.resolve().as_posix())

# Project modules
import exec
import gui_main
import postproc_flight

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
    configPath = pathlib.Path(__file__).parent / "config"

    if not(args.headless):
        gui_main.exec()
    elif inputPath is not None:
        exec.run(inputPath, outputPath, configPath)
    else:
        postproc_flight.postproc(outputPath)
