#------------------------------------------------------------#
# High Power Rocketry - Flight Simulation
# GNU General Public License v3.0
# Copyright (C) 2019 Roshan Doddanavar
# https://rdoddanavar.github.io
#------------------------------------------------------------#
#     ___       ___       ___       ___       ___       ___
#    /\__\     /\  \     /\  \     /\  \     /\  \     /\__\  
#   /:/__/_   /::\  \   /::\  \   /::\  \   _\:\  \   /::L_L_
#  /::\/\__\ /::\:\__\ /::\:\__\ /\:\:\__\ /\/::\__\ /:/L:\__\
#  \/\::/  / \/\::/  / \;:::/  / \:\:\/__/ \::/\/__/ \/_/:/  /
#    /:/  /     \/__/   |:\/__/   \::/  /   \:\__\     /:/  /
#    \/__/               \|__|     \/__/     \/__/     \/__/  
# 
#------------------------------------------------------------#
# Path:
#     hpr-sim/src/util/util_yaml.py
# Created:
#     2019-05-25
# Type:
#     Python3 module
# Description:
#     YAML helper functions
# Functions:
#     load
#     process
# Classes:
#     None
#------------------------------------------------------------#

# System modules
import sys  # System utilities
import pdb  # Python debugger
import yaml # YAML parser

def load(filePath):

    '''
    Parses YAML document using filepath, returns dict.
    '''

    stream   = open(filePath, 'r')
    fileDict = yaml.safe_load(stream)

    return fileDict

def process(fileDict):

    '''
    Processes pyYAML output; resolves references and evaluates arithmetic expressions. 
    '''

    pass

if __name__ == "__main__":
    
    pass

