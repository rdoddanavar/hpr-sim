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
    hpr-sim/src/preproc/input.h
Created:
    2019-05-17
Type:
    C++ header
Description:
    Header for input classes: Input, Sim, Param, & Name
Input:
    None
Output:
    None
Dependencies:
    hpr-sim/
        src/
            preproc/
                input.cpp
//--------------------------------------------------------*/

#ifndef INPUT_H
#define INPUT_H

// Installed libraries
#include <pybind11/pybind11.h>
#include <string>

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
        std::string dist;

        // Function(s)
        void checkValue();
        void checkUnit();
        void checkDist();

        // Constructor(s)
        Param(); // Default 
        Param(float);
};

class Name 
{
    public:

        // Data
        std::string value;
        bool path;

        // Function(s)
        bool checkPath(); 

        // Constructor(s)
        Name();
        Name(std::string);
};

struct Sim
{
    Name modeRand;
    Name modeProc;
    Param numMC;
};

struct Input
{
    Sim sim;
};

// Binding code
PYBIND11_MODULE(input, m)
{
    m.doc() = "sim input module"; // Optional module docstring

    py::class_<Param>(m, "Param")
        .def(py::init<>())
        .def(py::init<float>())
        .def("checkValue", &Param::checkValue)
        .def("checkUnit", &Param::checkUnit)
        .def("checkDist", &Param::checkDist)
        .def_readwrite("value", &Param::value)
        .def_readwrite("min", &Param::min)
        .def_readwrite("max", &Param::max)
        .def_readwrite("unit", &Param::unit)
        .def_readwrite("dist", &Param::dist);

    py::class_<Name>(m, "Name")
        .def(py::init<>())
        .def(py::init<std::string>())
        .def("checkPath", &Name::checkPath)
        .def_readwrite("value", &Name::value)
        .def_readwrite("path", &Name::path);

    py::class_<Sim>(m, "Sim")
        .def(py::init<>())
        .def_readwrite("modeRand", &Sim::modeRand)
        .def_readwrite("modeProc", &Sim::modeProc)
        .def_readwrite("numMC", &Sim::numMC);

    py::class_<Input>(m, "Input")
        .def(py::init<>())
        .def_readwrite("sim", &Input::sim);
}

#endif 