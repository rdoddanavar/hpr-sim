#include <pybind11/pybind11.h>
#include <string>

namespace py = pybind11;

// group ---> input / file --> dist?

// Class definition 
class Input 
{
    double value;
    double min;
    double max;
};

// Binding code
PYBIND11_MODULE(input, m)
{
    m.doc() = "sim input module"; // optional module docstring
    py::class_<Input>(m, "Input");
}