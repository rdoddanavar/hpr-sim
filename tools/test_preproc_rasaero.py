# System modules
import sys
import pathlib

# Path modifications
paths = ["../src/preproc"]

for item in paths:
    addPath = pathlib.Path(__file__).parent / item
    sys.path.append(str(addPath.resolve()))

# Project modules
import preproc_aerodynamics

#------------------------------------------------------------------------------#

if __name__ == "__main__":

    inputPath  = sys.argv[1]
    outputPath = sys.argv[2]
    machMax    = float(sys.argv[3])

    preproc_aerodynamics.load_rasaero(inputPath, outputPath, machMax)