# System modules
import sys # System utilities
import pdb # Python debugger
import pathlib

# Path modifications
paths = ["../../build/src", "../preproc", "../util"]

for item in paths:
    addPath = pathlib.Path(__file__).parent / item
    sys.path.append(str(addPath.resolve()))

# Project modules
import preproc_input
import util_yaml

# Module variables
configPathRel = "../../config/config_input.yml"

#------------------------------------------------------------------------------#

def exec(inputPath):

    # Parse CLI

    # Pre-processing
    configPath = pathlib.Path(__file__).parent / configPathRel
    configPath = str(configPath.resolve())
    configDict = util_yaml.load(configPath)

    inputDict = util_yaml.load(inputPath)
    inputDict = util_yaml.process(inputDict)

    preproc_input.process(inputDict, configDict)

    # Sim execution 

    # Post-processing

if __name__ == "__main__":

    inputPath = sys.argv[1]

    exec(inputPath)