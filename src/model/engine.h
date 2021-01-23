#pragma once

// System libraries

// Project headers
#include "pybind11/pybind11.h"
#include "pybind11/numpy.h"
#include "model.h"
#include "gsl/interpolation/gsl_interp.h"
#include "gsl/interpolation/gsl_spline.h"

namespace py = pybind11;

// Class definitions

class Engine : public Model
{
    public:

        // Data
        gsl_spline* spline;
        gsl_interp_accel* acc;

        // Function(s)
        void initialize(py::array_t<double>, py::array_t<double>, py::array_t<double>);
        void update(double) override;

        //Engine();  // Constructor
        ~Engine(); // Destructor
};

//---------------------------------------------------------------------------//

// BINDING CODE

// See model.h for module delcaration

void init_Engine(py::module &m)
{

    py::class_<Engine, Model>(m, "Engine")
        .def(py::init<>())
        .def("initialize", &Engine::initialize)
        .def("update", &Engine::update);

}