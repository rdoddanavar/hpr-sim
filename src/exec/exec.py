# Builtin modules
import sys # System utilities

# Path modifications
sys.path.insert(0, "../preproc/")
sys.path.insert(0, "../util/")
sys.path.insert(0, "../../build/src")

# Program modules
import preproc_input

def exec(inputPath):

    # Parse CLI

    # Pre-processing
    preproc_input.preproc_input(inputPath)

    # Sim execution 

    # Post-processing

if __name__ == "__main__":

    inputPath = sys.argv[1]
    exec(inputPath)