#!/usr/bin/env python3

# System modules
import sys
import argparse
import pathlib

# Path modifications
paths = ["./src/exec", "./src/postproc"]

for item in paths:
    addPath = pathlib.Path(__file__).parent / item
    sys.path.append(str(addPath.resolve()))

# Project modules
import exec
import postproc_flight

#------------------------------------------------------------------------------#

if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument('-i', "--input" , type=str, help="Input file path")
    parser.add_argument('output', type=str, help="Output file path")

    args       = parser.parse_args()
    inputPath  = args.input
    outputPath = args.output

    if inputPath is not None:
        exec.exec(inputPath, outputPath)
    else:
        postproc_flight.postproc(outputPath)