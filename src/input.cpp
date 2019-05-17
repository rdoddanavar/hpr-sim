// Installed libraries
#include <pybind11/pybind11.h>
#include <string>

// Project libraries
#include <input.h>

namespace py = pybind11;

// group ---> input / file --> dist?

// Class definition 
class Input 
{
    public:

        // Function(s)
        Input(); // Constructor 

        // could be optional --> sets "value" for non-dist params
        // use default value for "value" if empty argument
        // Find way to accept string or float?

        // Data
        double value;
        double min;
        double max;
        std::string unit;
        std::string dist; 
};

// Member function definition(s)

// Constructor
Input::Input(void)
{
    value = ?;
    min   = 0.0;
    max   = 0.0;
    unit  = "none";
    dist  = "none";

}

// Binding code
PYBIND11_MODULE(input, m)
{
    m.doc() = "sim input module"; // Optional module docstring
    py::class_<Input>(m, "Input")
        .def(py::init<>())
        .def_readwrite("value", &Input::value)
        .def_readwrite("min", &Input::min)
        .def_readwrite("max", &Input::max);
}