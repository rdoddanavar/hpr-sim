#pragma once

// System libraries
#include <string>
#include <vector>
#include <set>
#include <map>
#include <cstdio>

// External libraries
#include "pybind11/pybind11.h"
#include "pybind11/stl.h" // For std::map; induces overhead, remove if possible
#include "pybind11/numpy.h"
#include "gsl/interpolation/gsl_interp.h"
#include "gsl/interpolation/gsl_spline.h"
#include "eigen/Eigen/Core"

// Project headers
// <none>

namespace py = pybind11;

using stateMap = std::map<std::string, double>;

//---------------------------------------------------------------------------//

class Model
{
    
    public: 

        // Data
        stateMap state;
        stateMap stateInit;
        std::set<Model*> depModels; // std::set enforces unique elements

        // Function(s)
        virtual void update(stateMap& gState) = 0; // Pure virtual
        virtual void update_gState(stateMap& gState) = 0; // Pure virtual

        void reset()
        {
            state = stateInit;
        };

        void add_dep(Model* dep)
        {
            // std::set will quietly ignore duplicate elements
            // Should an exception be raised when insertion is skipped?
            depModels.insert(dep);
        }

    protected:

        void update_deps(stateMap& gState)
        {
            for (const auto& dep : depModels)
            {
                dep->update(gState);
            }
        }

};

//---------------------------------------------------------------------------//

class Engine : public Model
{
    
    public:

        // Function(s)
        void initialize(py::array_t<double> time  , 
                        py::array_t<double> thrust, 
                        py::array_t<double> mass  );

        void update(stateMap& gState) override;
        void update_gState(stateMap& gState) override;

        ~Engine(); // Destructor

    private:

        // Data
        gsl_spline       *thrustSpline, *massSpline;
        gsl_interp_accel *thrustAcc   , *massAcc   ;

};

//---------------------------------------------------------------------------//

class Geodetic : public Model
{
    public:

        // Data

        // Function(s)
        void initialize(double phi);
        void update(stateMap& gState) override;
        void update_gState(stateMap& gState) override;

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

class EOM : public Model
{

    public:

        void initialize();
        void update(stateMap& gState) override;
        void update_gState(stateMap& gState) override;

        //----------------------------------------------//
        void test(double timeEval);
        std::map<std::string, double> tState;

    private:

        // Data
        Eigen::Vector3d force;  // Force  [N]
        Eigen::Vector3d moment; // Moment [N*m]

        Eigen::Vector3d linAcc; // Linear acceleration [m/s^2]
        Eigen::Vector3d linVel; // Linear velocity     [m/s]
        Eigen::Vector3d linPos; // Linear position     [m]

        Eigen::Vector3d angAcc; // Angular acceleration [rad/s^2]
        Eigen::Vector3d angVel; // Angular velocity     [rad/s]
        Eigen::Vector3d angPos; // Angular position     [rad]

};