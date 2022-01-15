// System libraries

// External libraries
#include "pybind11/pybind11.h"

// Project headers
#include "model.h"

namespace py = pybind11;

//---------------------------------------------------------------------------//

// BINDING CODE

// Expose derived classes
void bind_Test         (py::module_ &);
void bind_Engine       (py::module_ &);
void bind_Mass         (py::module_ &);
void bind_Geodetic     (py::module_ &);
void bind_Atmosphere   (py::module_ &);
void bind_Aerodynamics (py::module_ &);
void bind_EOM          (py::module_ &);
void bind_Flight       (py::module_ &);

PYBIND11_MODULE(model, m)
{
    
    m.doc() = "Simulation Model Classes"; // Optional module docstring

    // Exposing base class necessary for derived construction
    // Base methods exposed once, automatically available to dervied in python
    py::class_<Model>(m, "Model")
        .def("add_dep", &Model::add_dep)
        .def("init_state", static_cast<void (Model::*)()>(&Model::init_state))
        .def("update", &Model::update);

    // Expose derived classes
    bind_Test         (m);
    bind_Engine       (m);
    bind_Mass         (m);
    bind_Geodetic     (m);
    bind_Atmosphere   (m);
    bind_Aerodynamics (m);
    bind_EOM          (m);
    bind_Flight       (m);

}

//---------------------------------------------------------------------------//

void bind_Test(py::module_ &m)
{

    py::class_<Test, Model>(m, "Test")
        .def(py::init<>())
        .def("init", &Test::init)
        .def("set_state_data", &Test::set_state_data)
        .def("get_state_data", &Test::get_state_data);

}

//---------------------------------------------------------------------------//

void bind_Engine(py::module_ &m)
{

    py::class_<Engine, Model>(m, "Engine")
        .def(py::init<>())
        .def("init", &Engine::init);

}

//---------------------------------------------------------------------------//

void bind_Mass(py::module_ &m)
{

    py::class_<Mass, Model>(m, "Mass")
        .def(py::init<>())
        .def("init", &Mass::init);

}

//---------------------------------------------------------------------------//

void bind_Geodetic(py::module_ &m)
{

    py::class_<Geodetic, Model>(m, "Geodetic")
        .def(py::init<>())
        .def("init", &Geodetic::init);

}

//---------------------------------------------------------------------------//

void bind_Atmosphere(py::module_ &m)
{

    py::class_<Atmosphere, Model>(m, "Atmosphere")
        .def(py::init<>())
        .def("init", &Atmosphere::init);

}

//---------------------------------------------------------------------------//

void bind_Aerodynamics(py::module_ &m)
{

    py::class_<Aerodynamics, Model>(m, "Aerodynamics")
        .def(py::init<>())
        .def("init", &Aerodynamics::init);

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
        .def("init", &Flight::init)
        .def_static("set_telem", &Flight::set_telem)
        .def_readonly_static("telemFieldsDefault", &Flight::telemFieldsDefault)
        .def("write_telem", &Flight::write_telem)
        .def("write_stats", &Flight::write_stats)
        .def_readwrite("odeSolver", &Flight::odeSolver);

    py::class_<OdeSolver>(m, "OdeSolver")
        .def(py::init<>())
        .def("set_method", &OdeSolver::set_method)
        .def_readwrite("hStart", &OdeSolver::hStart)
        .def_readwrite("epsAbs", &OdeSolver::epsAbs)
        .def_readwrite("epsRel", &OdeSolver::epsRel);

}