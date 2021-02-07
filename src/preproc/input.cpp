/*
High Power Rocketry - Flight Simulation
MIT License
Copyright (c) 2019 Roshan Doddanavar
https://rdoddanavar.github.io

Path:
    hpr-sim/src/preproc/input.cpp
Created:
    2019-05-16
Type:
    C++11 source
Description:
    Implementation for input classes.
Functions:
    None
Classes:
    Input
    Sim
    Param
    Name
Dependencies:
    hpr-sim/src/preproc/input.h
*/

// System libraries
#include <iostream>
#include <fstream>
#include <string>

// Project headers
#include "input.h"

// "Param" class
Param::Param() : value(0.0), min(0.0), max(0.0), unit(""), quantity(""), dist("") {}

bool Param::check_value()
{
    
    bool cond = (value >= min) && (value <= max);
    return cond;

}

// "Name" class
Name::Name() : value(""), isPath(false) {}

bool Name::check_path()
{

    if (isPath)
    {
        std::ifstream file(value);
        return file.good();
    }
    else
    {
        return true;
    }
}