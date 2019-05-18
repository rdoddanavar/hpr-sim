#ifndef INPUT_H
#define INPUT_H

// Installed libraries
#include <pybind11/pybind11.h>
#include <string>

namespace py = pybind11;

// Class definition 
class Input 
{
    
    private:

    public:

        // Data
        double value;
        double min;
        double max;
        std::string unit;

        // Function(s)
        Input();
        Input(float);

};

// Binding code
PYBIND11_MODULE(input, m)
{
    m.doc() = "sim input module"; // Optional module docstring
    py::class_<Input>(m, "Input")
        .def(py::init<>())
        .def(py::init<float>())
        .def_readwrite("value", &Input::value)
        .def_readwrite("min", &Input::min)
        .def_readwrite("max", &Input::max)
        .def_readwrite("unit", &Input::unit);
}

#endif 