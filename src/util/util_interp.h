#pragma once

// System libraries

// Project headers
#include "pybind11/pybind11.h"
#include "gsl/interpolation/gsl_interp.h"
#include "gsl/interpolation/gsl_spline.h"

namespace py = pybind11;

// Prototypes

//void interp1d_init(const double x[], const double y[], size_t n, gsl_spline* &spline, gsl_interp_accel* &acc);
//double interp1d_eval(gsl_spline* spline, double xq, gsl_interp_accel* acc);

class Model
{

    public:

        // Data
        gsl_spline* spline;
        gsl_interp_accel* acc;

        std::vector<double> x;
        std::vector<double> y;

        // Functions
        void initialize();
        double update(double xq);

        ~Model();

};

// Binding code

PYBIND11_MODULE(test_gsl, m)
{
    m.doc() = "GNU GSL test"; // Optional module docstring

    //m.def("interp1d_init", &interp1d_init, "GNU GSL interp1d init");
    //m.def("interp1d_eval", &interp1d_eval, "GNU GSL interp1d eval");

    py::class_<Model>(m, "Model")
        .def(py::init<>())
        .def("initialize", &Model::initialize)
        .def("update", &Model::update);

}