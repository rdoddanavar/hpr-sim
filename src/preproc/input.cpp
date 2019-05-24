/*--------------------------------------------------------//
High Power Rocketry - Flight Simulation
GNU General Public License v3.0
Roshan Doddanavar
https://rdoddanavar.github.io
//--------------------------------------------------------//
    ___       ___       ___       ___       ___       ___
   /\__\     /\  \     /\  \     /\  \     /\  \     /\__\  
  /:/__/_   /::\  \   /::\  \   /::\  \   _\:\  \   /::L_L_
 /::\/\__\ /::\:\__\ /::\:\__\ /\:\:\__\ /\/::\__\ /:/L:\__\
 \/\::/  / \/\::/  / \;:::/  / \:\:\/__/ \::/\/__/ \/_/:/  /
   /:/  /     \/__/   |:\/__/   \::/  /   \:\__\     /:/  /
   \/__/               \|__|     \/__/     \/__/     \/__/ 

//--------------------------------------------------------//
Path:
    hpr-sim/src/preproc/input.cpp
Created:
    2019-05-16
Type:
    C++ source
Description:
    Implementation for input classes: Input, Sim, Param, & Name
Input:
    None
Output:
    None
Dependencies:
    hpr-sim/
        src/
            preproc/
                input.h
//--------------------------------------------------------*/

// Installed libraries
#include <iostream>
#include <fstream>
#include <string>

// Project libraries
#include "input.h"

// "Param" class
Param::Param() : value(5.0), min(0.0), max(0.0), unit(""), dist("") {}

Param::Param(float val) : value(val), min(0.0), max(0.0), unit(""), dist("") {}

void Param::checkValue()
{
    std::cout << "checkValue" << std::endl;
}

void Param::checkUnit()
{
    std::cout << "checkUnit" << std::endl;
}

void Param::checkDist()
{
    std::cout << "checkDist" << std::endl;
}

// "Name" class
Name::Name() : value(""), path(false) {}

Name::Name(std::string val) : value(val), path(false) {}

bool Name::checkPath()
{
    std::ifstream file(value);
    return file.good();
}

// "Input" class 