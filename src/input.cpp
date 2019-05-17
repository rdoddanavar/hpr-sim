#include <pybind11/pybind11.h>
#include <string>

namespace py = pybind11;

// group ---> input / file --> dist?

// Class definition 
class Input 
{
    public:

        // Function(s)
        Input(); // Constructor 

        // Data
        double value = 0.0;
        double min   = 0.0;
        double max   = 0.0;
};

// Member function definition(s)
Input::Input(void)
{
    // Empty constructor 
}

// Binding code
PYBIND11_MODULE(input, m)
{
    m.doc() = "sim input module"; // optional module docstring
    py::class_<Input>(m, "Input")
        .def(py::init<>())
        .def_readwrite("value", &Input::value)
        .def_readwrite("min", &Input::min)
        .def_readwrite("max", &Input::max);
}