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

inputPath  = "./input/unit_test.yml"
outputPath = "./output"

if inputPath is not None:
    exec.exec(inputPath, outputPath)
else:
    postproc_flight.postproc(outputPath)