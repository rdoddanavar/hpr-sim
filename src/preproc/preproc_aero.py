'''
High Power Rocketry - Flight Simulation
MIT License
Copyright (C) 2019 Roshan Doddanavar
https://rdoddanavar.github.io

Path:
    hpr-sim/src/preproc/preproc_engine.py
Created:
    2020-12-22
Type:
    Python3 module
Description:
    < >
Functions:
    < >
Classes:
    < >
Dependencies:
    < >
'''

# System modules
import sys
import glob
import numpy as np

#------------------------------------------------------------------------------#

# See RASAero II User Manual pg. 90-93 for CDDataFile.txt format

def load(inputPath):

    # Collect data files
    filePaths  = glob.glob(inputPath + "CDDataFile*.txt")
    blockCount = 0

    # Setup data dimensions
    machData  = np.arange(0.01, 25.01, 0.01)
    alphaData = np.zeros(len(filePaths))

    nMach  = len(machData)
    nAlpha = len(alphaData)

    # Initialize data
    cpData         = np.zeros((nMach, nAlpha))
    cdDataPowerOff = np.zeros((nMach, nAlpha))
    cdDataPowerOn  = np.zeros((nMach, nAlpha))
    clDataPowerOff = np.zeros((nMach, nAlpha))
    clDataPowerOn  = np.zeros((nMach, nAlpha))

    iMach  = 0
    iAlpha = 0

    for filePath in filePaths:
        with open(filePath, 'r') as file:
            for line in file.read().splitlines():
                
                words = line.split()

                if not words:
                    # Empty text line, reset count
                    blockCount = 0

                elif all(word.replace('.', '').isdecimal() for word in words):
                    
                    # Valid data line
                    blockCount += 1

                    # TODO: replace if-elif w/ match from python 3.10
                    if blockCount == 1:
                        continue
                    elif blockCount == 2:
                        continue
                    elif blockCount == 3:
                        continue
                    elif blockCount == 4:
                        
                        alphaData[iAlpha]     = float(words[1])
                        cpData[iMach][iAlpha] = float(words[3]) 

                    elif blockCount == 5:

                        clDataPowerOff[iMach][iAlpha] = float(words[2])
                        cdDataPowerOff[iMach][iAlpha] = float(words[3])

                    elif blockCount == 6:

                        clDataPowerOn[iMach][iAlpha] = float(words[2])
                        cdDataPowerOn[iMach][iAlpha] = float(words[3])

                        # Block finished, increment mach no.
                        iMach += 1

                else:
                    # Contains alpha characters, ignore
                    continue
        
        # File finished, reset mach no., increment alpha
        iMach   = 0
        iAlpha += 1

    # TODO: alpha data is not sorted! File order is arbitrary atm

    # Convert alpha
    np.deg2rad(alphaData)

    return (machData, alphaData, cpData, clDataPowerOff, cdDataPowerOff, clDataPowerOn, cdDataPowerOn)

#------------------------------------------------------------------------------#

if __name__ == "__main__":

    #inputPath = sys.argv[1]
    inputPath = "/home/roshan/Documents/hpr-sim/patriot/CDDataFile/"
    load(inputPath)
