# System modules
import sys      # System utilities
import pdb      # Python debugger
import logging  # Logging tools
import argparse # CLI parsing

# Path modifications

# Program modules
sys.path.insert(0, "../preproc/")
sys.path.insert(0, "../util/")

import preproc_input

# Pybind11 modules
sys.path.insert(0, "../../build/src")

def exec(inputPath):

    # Parse CLI

    # Pre-processing
    preproc_input.preproc_input(inputPath)

    # Sim execution 

    # Post-processing

if __name__ == "__main__":

    inputPath = sys.argv[1]
    exec(inputPath)