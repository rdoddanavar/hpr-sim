#pragma once

// System libraries

// External libraries
#include "pybind11/pybind11.h"
#include "pybind11/numpy.h"
#include "gsl/interpolation/gsl_interp.h"
#include "gsl/interpolation/gsl_spline.h"

// Project headers
#include "model.h"

namespace py = pybind11;

// Class definitions

class Engine : public Model
{
    public:

        // Data
        gsl_spline *thrustSpline, *massSpline;
        gsl_interp_accel *thrustAcc, *massAcc;

        // Function(s)
        void initialize(py::array_t<double>, py::array_t<double>, py::array_t<double>);
        void update(double) override;

        //Engine();  // Constructor
        ~Engine(); // Destructor
};

//---------------------------------------------------------------------------//

// BINDING CODE

// See src/model/model.h for module delcaration

void init_Engine(py::module &m)
{

    py::class_<Engine, Model>(m, "Engine")
        .def(py::init<>())
        .def("initialize", &Engine::initialize)
        .def("update", &Engine::update);

}