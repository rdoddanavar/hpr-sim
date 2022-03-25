
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
    alphaData = np.zeros(len(filePaths))
    machData  = np.arange(0.01, 25.01, 0.01)

    nAlpha = len(alphaData)
    nMach  = len(machData)

    # Initialize data
    aeroData = {}

    aeroData["cpTotal"]    = np.zeros((nAlpha, nMach))
    aeroData["clPowerOff"] = np.zeros((nAlpha, nMach))
    aeroData["cdPowerOff"] = np.zeros((nAlpha, nMach))
    aeroData["clPowerOn"]  = np.zeros((nAlpha, nMach))
    aeroData["cdPowerOn"]  = np.zeros((nAlpha, nMach))
    
    iAlpha = 0
    iMach  = 0

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
                        
                        alphaData[iAlpha] = float(words[1])
                        
                        aeroData["cpTotal"][iAlpha][iMach] = float(words[3]) 

                    elif blockCount == 5:

                        aeroData["clPowerOff"][iAlpha][iMach] = float(words[2])
                        aeroData["cdPowerOff"][iAlpha][iMach] = float(words[3])

                    elif blockCount == 6:

                        aeroData["clPowerOn"][iAlpha][iMach] = float(words[2])
                        aeroData["cdPowerOn"][iAlpha][iMach] = float(words[3])

                        # Block finished, increment mach no.
                        iMach += 1

                else:
                    
                    # Contains alpha characters, ignore
                    continue
        
        # File finished, increment alpha, reset mach no. 
        iAlpha += 1
        iMach   = 0

    # Sort and convert angle-of-attack
    iAlphaSort = np.argsort(alphaData)
    alphaData  = np.deg2rad(alphaData[iAlphaSort])

    for key in aeroData.keys():

        # Sort data to match order of alphaData (increasing)
        # Swap dimensions: data[iAlpha, iMach) -> data[iMach, iAlpha]
        aeroData[key] = np.transpose(aeroData[key][iAlphaSort])

    # TODO: output structure like object
    return (machData, alphaData, aeroData)