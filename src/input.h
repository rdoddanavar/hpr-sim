#ifndef INPUT_H
#define INPUT_H

// Installed libraries
#include <pybind11/pybind11.h>
#include <string>

namespace py = pybind11;

// Class definition 
class Param 
{
    
    private:

    public:

        // Data
        double value;
        double min;
        double max;
        std::string unit;

        // Function(s)
        Param();
        Param(float);

};

// Binding code
PYBIND11_MODULE(input, m)
{
    m.doc() = "sim input module"; // Optional module docstring
    py::class_<Param>(m, "Param")
        .def(py::init<>())
        .def(py::init<float>())
        .def_readwrite("value", &Param::value)
        .def_readwrite("min", &Param::min)
        .def_readwrite("max", &Param::max)
        .def_readwrite("unit", &Param::unit);
}

#endif 