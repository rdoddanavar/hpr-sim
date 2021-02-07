#pragma once

// System libraries
#include <string>
#include <vector>
#include <map>
#include <cstdio>

// External libraries
#include "pybind11/pybind11.h"
#include "pybind11/stl.h" // For std::map; induces overhead, remove if possible

// Project headers
// <none>

namespace py = pybind11;

//---------------------------------------------------------------------------//

class Model
{
    public: 

        // Data
        //std::vector<Model*> depModels; // don't forget to use "new" keyword for mem alloc
        std::map<std::string, double> state;
        std::map<std::string, double> stateInit;

        // Function(s)
        virtual void update(double) = 0; // Pure virtual
        void reset();
        //void update_deps();

        // Constructor(s)
        //Model(){;};
};


void Model::reset()
{
    state = stateInit;
}


/*
void Model::update_deps()
{

    // Iterates over model dependencies, updates internal states 

    for (auto dep : depModels)
    {
        dep->update();
    }
}
*/

/*
void Model::add_dep(Model dep)
{
    // add pointer from model to depModels?
    depModels.push_back(&dep)
}

//---------------------------------------------------------------------------//

// BINDING CODE

/*
Release Notes (pybind11 v2.6.0)

py::module was renamed py::module_ to avoid issues with C++20 when used unqualified, but an alias
py::module is provided for backward compatibility. #2489
*/

// Exposed derived classes
void init_Engine(py::module &);

PYBIND11_MODULE(model, m)
{
    
    m.doc() = "Simulation Model Classes"; // Optional module docstring

    // Exposing base class necessary for dervied construction
    // Base methods exposed once, automatically available to dervied in python
    py::class_<Model>(m, "Model")
        .def("update", &Model::reset)
        .def_readonly("state", &Model::state);

    // Exposed derived classes
    init_Engine(m);

}