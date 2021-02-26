#pragma once

// System libraries
#include <string>
#include <vector>
#include <map>
#include <cstdio>

// External libraries
#include "pybind11/pybind11.h"
#include "pybind11/stl.h" // For std::map; induces overhead, remove if possible
#include "pybind11/numpy.h"

// Project headers
#include "gsl/interpolation/gsl_interp.h"
#include "gsl/interpolation/gsl_spline.h"

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
        void reset()
        {
            state = stateInit;
        };
        //void update_deps();

        // Constructor(s)
        //Model(){;};
};


// void Model::reset()
// {
//     state = stateInit;
// }


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
*/

//---------------------------------------------------------------------------//

class Engine : public Model
{
    public:

        // Data
        gsl_spline       *thrustSpline, *massSpline;
        gsl_interp_accel *thrustAcc   , *massAcc   ;

        // Function(s)
        void initialize(py::array_t<double> time  , 
                        py::array_t<double> thrust, 
                        py::array_t<double> mass  );

        void update(double timeEval) override;

        //Engine();  // Constructor
        ~Engine(); // Destructor
};

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