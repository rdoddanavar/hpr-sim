#pragma once

// System libraries

// External libraries
#include "pybind11/pybind11.h"

// Project headers
#include "model.h"

namespace py = pybind11;

//---------------------------------------------------------------------------//

class Geodetic : public Model
{
    public:

        // Data

        // Function(s)
        void initialize(double phi);
        void update(double altEval) override;

    private:

        // Data
        double phi;
        double gamE;
        double k;
        double e;
        double a;
        double f;
        double m;

        // Function(s)
        double wgs84(double h);

};

//---------------------------------------------------------------------------//

// BINDING CODE

// See src/model/model.h for module declaration

void init_Geodetic(py::module &m)
{

    py::class_<Geodetic, Model>(m, "Geodetic")
        .def(py::init<>())
        .def("initialize", &Geodetic::initialize)
        .def("update"    , &Geodetic::update    );

}