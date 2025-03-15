// System libraries

// External libraries
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

// Project headers
#include "model.h"
#include "telem.h"

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
void bind_Telem        (py::module_ &);

PYBIND11_MODULE(model, m)
{

    m.doc() = "Simulation Model Classes"; // Optional module docstring

    // Exposing base class necessary for derived construction
    // Base methods exposed once, automatically available to dervied in python
    py::class_<Model>(m, "Model")
        .def("add_deps", &Model::add_deps)
        .def("init_state", &Model::init_state)
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
    bind_Telem        (m);

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
        .def("init", &Flight::init);

}

//---------------------------------------------------------------------------//

void bind_Telem(py::module_ &m)
{
    py::class_<Telem>(m, "Telem")
        .def(py::init<>())
        .def("init", &Telem::init)
        .def("update", &Telem::update)
        .def("interp_boundary", &Telem::interp_boundary)
        .def("finalize", &Telem::finalize);
}
