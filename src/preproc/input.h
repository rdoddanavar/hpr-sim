/*
High Power Rocketry - Flight Simulation
MIT License
Copyright (c) 2019 Roshan Doddanavar
https://rdoddanavar.github.io

Path:
    hpr-sim/src/preproc/input.h
Created:
    2019-05-17
Type:
    C++11 header
Description:
    Declaration for input classes.
Functions:
    None
Classes:
    Input
    Sim
    Param
    Name
Dependencies:
    hpr-sim/src/preproc/input.cpp
*/

#pragma once

// System libraries
#include <string>

// Project headers
#include "pybind11/pybind11.h"

namespace py = pybind11;

// Class definition(s)

class Param 
{
    public:

        // Data
        float value;
        float min;
        float max;
        std::string unit;
        std::string quantity;
        std::string dist;

        // Function(s)
        bool check_value();

        // Constructor(s)
        Param(); // Default 
};

class Name 
{
    public:

        // Data
        std::string value;
        bool isPath;

        // Function(s)
        bool check_path(); 

        // Constructor(s)
        Name();
};

struct Sim
{
    Name session;
    Name modeRand;
    Name modeProc;
    Param numMC;
    Param seedMaster;
};

/*
struct Prop
{
    Param thrustEta;
    Name engineModel;
};
*/

struct Input
{
    Sim sim;
    //Prop prop;
};

// Binding code

PYBIND11_MODULE(input, m)
{
    m.doc() = "sim input module"; // Optional module docstring

    py::class_<Param>(m, "Param")
        .def(py::init<>())
        .def("check_value", &Param::check_value)
        .def_readwrite("value", &Param::value)
        .def_readwrite("min", &Param::min)
        .def_readwrite("max", &Param::max)
        .def_readwrite("unit", &Param::unit)
        .def_readwrite("quantity", &Param::quantity)
        .def_readwrite("dist", &Param::dist); 

    py::class_<Name>(m, "Name")
        .def(py::init<>())
        .def("check_path", &Name::check_path)
        .def_readwrite("value", &Name::value)
        .def_readwrite("isPath", &Name::isPath);

    py::class_<Sim>(m, "Sim")
        .def(py::init<>())
        .def_readwrite("session", &Sim::session)
        .def_readwrite("modeRand", &Sim::modeRand)
        .def_readwrite("modeProc", &Sim::modeProc)
        .def_readwrite("numMC", &Sim::numMC)
        .def_readwrite("seedMaster", &Sim::seedMaster);

    py::class_<Input>(m, "Input")
        .def(py::init<>())
        .def_readwrite("sim", &Input::sim);
}