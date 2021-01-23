#pragma once

// System libraries
#include <vector>

// Project headers
#include "pybind11/pybind11.h"
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

        std::vector<double> x;
        std::vector<double> y;

        // Function(s)
        void initialize() override;
        //void update(double xq) override; --> override fails, not same fun sig as parent?

        void update(double xq);

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