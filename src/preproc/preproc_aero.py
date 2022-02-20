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

    filePaths  = glob.glob(inputPath + "CDDataFile*.txt")
    blockCount = 0

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

                    elif blockCount == 2:
                
                    elif blockCount == 3:
                    elif blockCount == 4:
                    elif blockCount == 5:
                    elif blockCount == 6:


                else:
                    # Contains alpha characters, ignore
                    continue

#------------------------------------------------------------------------------#

if __name__ == "__main__":

    #inputPath = sys.argv[1]
    inputPath = "/home/roshan/Documents/hpr-sim/patriot/CDDataFile/"
    load(inputPath)
