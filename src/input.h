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

/*
struct Sim
{
    Param 
};

struct Input
{
    Group sim;
};
*/

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
}

#endif 