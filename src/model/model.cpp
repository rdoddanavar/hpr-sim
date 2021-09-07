// System libraries

// External libraries
#include "pybind11/pybind11.h"

// Project headers
#include "model.h"

namespace py = pybind11;

//---------------------------------------------------------------------------//

// BINDING CODE

/*
Current version - pybind11 v2.5.0

Release Notes (pybind11 v2.6.0):
    py::module_ was renamed py::module__ to avoid issues with C++20 when used unqualified,
    but an alias py::module_ is provided for backward compatibility. #2489
*/

// Expose derived classes
void bind_Engine   (py::module_ &);
void bind_Mass     (py::module_ &);
void bind_Geodetic (py::module_ &);
void bind_EOM      (py::module_ &);
void bind_Flight   (py::module_ &);

PYBIND11_MODULE(model, m)
{
    
    m.doc() = "Simulation Model Classes"; // Optional module docstring

    // Exposing base class necessary for derived construction
    // Base methods exposed once, automatically available to dervied in python
    py::class_<Model>(m, "Model")
        .def("add_dep", &Model::add_dep);

    // Expose derived classes
    bind_Engine   (m);
    bind_Mass     (m);
    bind_Geodetic (m);
    bind_EOM      (m);
    bind_Flight   (m);

}

//---------------------------------------------------------------------------//

void bind_Engine(py::module_ &m)
{

    py::class_<Engine, Model>(m, "Engine")
        .def(py::init<>())
        .def("init"  , &Engine::init  );

}

//---------------------------------------------------------------------------//

void bind_Mass(py::module_ &m)
{

    py::class_<Mass, Model>(m, "Mass")
        .def(py::init<>())
        .def("init"  , &Mass::init  );

}

//---------------------------------------------------------------------------//

void bind_Geodetic(py::module_ &m)
{

    py::class_<Geodetic, Model>(m, "Geodetic")
        .def(py::init<>())
        .def("init"  , &Geodetic::init  );

}

//---------------------------------------------------------------------------//

void bind_EOM(py::module_ &m)
{

    py::class_<EOM, Model>(m, "EOM")
        .def(py::init<>())
        .def("init"  , &EOM::init  );

}

//---------------------------------------------------------------------------//

void bind_Flight(py::module_ &m)
{

    py::class_<Flight, Model>(m, "Flight")
        .def(py::init<>())
        .def("init"  , &Flight::init)
        .def("write_telem", &Flight::write_telem)
        .def("update", &Flight::update)
        .def_readwrite("odeSolver", &Flight::odeSolver);

    py::class_<OdeSolver>(m, "OdeSolver")
        .def(py::init<>())
        .def("set_method", &OdeSolver::set_method)
        .def_readwrite("hStart", &OdeSolver::hStart)
        .def_readwrite("epsAbs", &OdeSolver::epsAbs)
        .def_readwrite("epsRel", &OdeSolver::epsRel);

}