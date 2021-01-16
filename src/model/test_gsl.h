#pragma once

// System libraries

// Project headers
#include "pybind11/pybind11.h"

namespace py = pybind11;

// Prototypes
float interp1(double xq);

// Binding code

PYBIND11_MODULE(test_gsl, m)
{
    m.doc() = "GNU GSL test"; // Optional module docstring

    m.def("interp1", &interp1, "GNU GSL interp1");
}