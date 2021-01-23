#pragma once

// System libraries
#include <string>
#include <vector>
#include <map>

// Project headers
#include "pybind11/pybind11.h"
#include "gsl/interpolation/gsl_interp.h"
#include "gsl/interpolation/gsl_spline.h"

namespace py = pybind11;

// Class definitions

class Model
{
    public: 

        // Data
        std::vector<Model*> depModels; // don't forget to use "new" keyword for mem alloc
        std::map<std::string, float> state;
        std::map<std::string, float> stateInit;

        // Function(s)
        virtual void initialize();
        void reset();
        virtual void update();
        void update_deps();

        // Constructor(s)
        Model();
};

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
        void update(double xq) override;

        Engine();  // Constructor
        ~Engine(); // Destructor
};

// Binding code

PYBIND11_MODULE(model, m)
{
    
    m.doc() = "GNU GSL test"; // Optional module docstring

    py::class_<Engine>(m, "Engine")
        .def(py::init<>())
        .def("initialize", &Engine::initialize)
        .def("update", &Engine::update);

}