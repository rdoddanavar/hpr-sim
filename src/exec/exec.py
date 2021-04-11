# System modules
import sys # System utilities
import pdb # Python debugger
from pathlib import Path

# Path modifications
paths = ["../../build/src", "../preproc", "../util"]

for item in paths:
    addPath = Path(__file__).parent / item
    sys.path.append(str(addPath.resolve()))

# Project modules
import preproc_input

#-----------------------------------------------------------------------------#

def exec(inputPath, configPath):

    # Parse CLI

    # Pre-processing
    preproc_input.load(inputPath,configPath)

    # Sim execution 

    # Post-processing

if __name__ == "__main__":

    inputPath  = sys.argv[1]
    configPath = Path(__file__).parent / "../../config/config_param.yaml"
    configPath = str(configPath.resolve())

    exec(inputPath, configPath)