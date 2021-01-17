#pragma once

// System libraries

// Project headers
#include "pybind11/pybind11.h"
#include "gsl/interpolation/gsl_interp.h"
#include "gsl/interpolation/gsl_spline.h"

namespace py = pybind11;

// Prototypes
void interp1d_init(const double x[], const double y[], size_t n, gsl_spline* spline, gsl_interp_accel* acc);
void test_init();
float interp1d_eval(double xq);

// Binding code

PYBIND11_MODULE(test_gsl, m)
{
    m.doc() = "GNU GSL test"; // Optional module docstring

    m.def("interp1d_init", &interp1d_init, "GNU GSL interp1d init");
    m.def("interp1d_eval", &interp1d_eval, "GNU GSL interp1d eval");

    m.def("test_init", &test_init, "test init");
}