# System modules
import sys
import numpy as np
import matplotlib.pyplot as plt

#-----------------------------------------------------------------------------#

def postproc(filePath):

    with open(filePath, 'r') as file:
        
        lines = file.read().splitlines()

if __name__ == "__main__":

    filePath  = sys.argv[1]
    exec(filePath)