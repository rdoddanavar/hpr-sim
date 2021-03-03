// System libraries

// External libraries
#include "pybind11/pybind11.h"

// Project headers
#include "model.h"

//---------------------------------------------------------------------------//

// BINDING CODE

/*
Current version - pybind11 v2.5.0

Release Notes (pybind11 v2.6.0):
    py::module was renamed py::module_ to avoid issues with C++20 when used unqualified,
    but an alias py::module is provided for backward compatibility. #2489
*/

// Exposed derived classes
void init_Engine(py::module &);
void init_Geodetic(py::module &);
void init_EOM(py::module &);

PYBIND11_MODULE(model, m)
{
    
    m.doc() = "Simulation Model Classes"; // Optional module docstring

    // Exposing base class necessary for derived construction
    // Base methods exposed once, automatically available to dervied in python
    py::class_<Model>(m, "Model")
        .def("reset", &Model::reset)
        .def_readonly("state", &Model::state);

    // Exposed derived classes
    init_Engine(m);
    init_Geodetic(m);
    init_EOM(m);

}

//---------------------------------------------------------------------------//

void init_Engine(py::module &m)
{

    py::class_<Engine, Model>(m, "Engine")
        .def(py::init<>())
        .def("initialize", &Engine::initialize)
        .def("update"    , &Engine::update    );

}

//---------------------------------------------------------------------------//

void init_Geodetic(py::module &m)
{

    py::class_<Geodetic, Model>(m, "Geodetic")
        .def(py::init<>())
        .def("initialize", &Geodetic::initialize)
        .def("update"    , &Geodetic::update    );

}

//---------------------------------------------------------------------------//

void init_EOM(py::module &m)
{

    py::class_<EOM, Model>(m, "EOM")
        .def(py::init<>())
        .def("update"    , &EOM::update  );

}