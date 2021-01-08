#pragma once

// System libraries

// Project headers
#include "pybind11/pybind11.h"
#include "gsl/gsl_interp.h"

namespace py = pybind11;

// Binding code

PYBIND11_MODULE(input, m)
{
    m.doc() = "GNU GSL test"; // Optional module docstring

    m.def("interp1", &interp1, "GNU GSL interp1");
}