# System modules
import sys # System utilities
import pdb # Python debugger

# Path modifications
sys.path.insert(0, "../../build/src")
sys.path.insert(0, "../preproc/")
sys.path.insert(0, "../util/")

# Program modules
import preproc_input

def exec(inputPath, configPath):

    # Parse CLI

    # Pre-processing
    preproc_input.load(inputPath,configPath)

    # Sim execution 

    # Post-processing

if __name__ == "__main__":

    inputPath  = sys.argv[1]
    configPath = sys.argv[2]

    exec(inputPath, configPath)