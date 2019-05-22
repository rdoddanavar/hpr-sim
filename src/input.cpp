// Installed libraries
#include <iostream>
#include <fstream>
#include <string>

// Project libraries
#include "input.h"

// "Param" class
Param::Param() : value(0.0), min(0.0), max(0.0), unit(""), dist("") {}

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