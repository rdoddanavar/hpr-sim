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
void bind_Engine(py::module &);
void bind_Geodetic(py::module &);
void bind_EOM(py::module &);

PYBIND11_MODULE(model, m)
{
    
    m.doc() = "Simulation Model Classes"; // Optional module docstring

    // Exposing base class necessary for derived construction
    // Base methods exposed once, automatically available to dervied in python
    py::class_<Model>(m, "Model")
        .def("add_dep", &Model::add_dep);

    // Exposed derived classes
    bind_Engine(m);
    bind_Geodetic(m);
    bind_EOM(m);

}

//---------------------------------------------------------------------------//

void bind_Engine(py::module &m)
{

    py::class_<Engine, Model>(m, "Engine")
        .def(py::init<>())
        .def("init"  , &Engine::init  )
        .def("update", &Engine::update);

}

//---------------------------------------------------------------------------//

void bind_Geodetic(py::module &m)
{

    py::class_<Geodetic, Model>(m, "Geodetic")
        .def(py::init<>())
        .def("init"  , &Geodetic::init  )
        .def("update", &Geodetic::update);

}

//---------------------------------------------------------------------------//

void bind_EOM(py::module &m)
{

    py::class_<EOM, Model>(m, "EOM")
        .def(py::init<>())
        .def("init"  , &EOM::init  )
        .def("update", &EOM::update);

}