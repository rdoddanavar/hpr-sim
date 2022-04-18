# System modules
import sys
import glob
import numpy as np

#------------------------------------------------------------------------------#

def load_csv(inputPath):
    
    inData = np.loadtxt(inputPath, delimiter=', ', skiprows=1)

    machData  = inData[:, 0]
    alphaData = inData[:, 1]
    
    aeroData = {}

    aeroData["cpTotal"]    = inData[:, 2]
    aeroData["clPowerOff"] = inData[:, 3]
    aeroData["cdPowerOff"] = inData[:, 4]
    aeroData["clPowerOn"]  = inData[:, 5]
    aeroData["cdPowerOn"]  = inData[:, 6]

    return machData, alphaData, aeroData

#------------------------------------------------------------------------------#

# See RASAero II User Manual pg. 90-93 for CDDataFile.txt format

def load_rasaero(inputPath, outputPath, machMax):

    # Collect data files
    filePaths  = glob.glob(inputPath + "CDDataFile*.txt")
    blockCount = 0

    # Setup data dimensions
    alphaData = np.zeros(len(filePaths))
    dMach     = 0.01
    machData  = np.arange(dMach, machMax+dMach, dMach)

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

                        if float(words[0]) > machMax:
                            break

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

    # Build output array for CSV archive
    nRow = nMach*nAlpha
    nCol = 2 + len(aeroData.keys())
    outData = np.zeros((nRow, nCol))

    for iMach in range(nMach):
        for iAlpha in range(nAlpha):

            iData = iMach*nAlpha + iAlpha

            outData[iData][0] = machData[iMach]
            outData[iData][1] = alphaData[iAlpha]

            outData[iData][2] = aeroData["cpTotal"][iMach][iAlpha]
            outData[iData][3] = aeroData["clPowerOff"][iMach][iAlpha]
            outData[iData][4] = aeroData["cdPowerOff"][iMach][iAlpha]
            outData[iData][5] = aeroData["clPowerOn"][iMach][iAlpha]
            outData[iData][6] = aeroData["cdPowerOn"][iMach][iAlpha]

    headerStr = ", ".join(["mach", 'alpha', "cpTotal", "clPowerOff", "cdPowerOff", "clPowerOn", "cdPowerOn"])
    np.savetxt(outputPath, outData, fmt="%.3e", delimiter=", ", header=headerStr, comments='')