
// External libraries
#include "pybind11/pybind11.h"

// Project headers
#include "telem.h"

namespace py = pybind11;

//----------------------------------------------------------------------------

PYBIND11_MODULE(telem, m)
{

    m.doc() = "Telemetry class"; // Optional module docstring

    py::class_<Telem>(m, "Telem")
        .def(py::init<const std::string&, const int&, const std::string&, const std::string&>());

}
