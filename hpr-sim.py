#!/usr/bin/env python3

# System modules
import sys
import os
import argparse
import pathlib

# Path modifications
paths = ["build/src", "src/exec", "src/preproc", "src/postproc", "src/util"]

for item in paths:
    addPath = pathlib.Path(__file__).parent / item
    sys.path.append(addPath.resolve().as_posix())

# Project modules
import exec
import postproc_flight
import util_misc

#------------------------------------------------------------------------------#

if __name__ == "__main__":

    # TODO: mp.freeze_support() # Need this for pyinstaller

    # Parse CLI
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', "--input" , type=str, help="Input file path")
    parser.add_argument('output', type=str, help="Output file path")

    args       = parser.parse_args()
    inputPath  = args.input
    outputPath = args.output
    configPath = pathlib.Path(__file__).parent / "config"

    # Get version
    cmakePath = pathlib.Path(__file__).parent / "build" / "CMakeCache.txt"
    version   = util_misc.get_cmake_cache(cmakePath.resolve().as_posix(), "CMAKE_PROJECT_VERSION")

    if os.name == "nt":

        # Explicitly add path to libstdc++
        compilerPath       = util_misc.get_cmake_cache(cmakePath.resovle().as_posix(), "CMAKE_CXX_COMPILER")
        compilerPathParent = pathlib.Path(compilerPath).parent

        os.add_dll_directory(compilerPathParent)

    if inputPath is not None:
        exec.set_version(version)
        exec.exec(inputPath, outputPath, configPath)
    else:
        postproc_flight.postproc(outputPath)
