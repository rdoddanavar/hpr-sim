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

//---------------------------------------------------------------------------//

class Model
{
    
    public: 

        // Data
        std::set<Model*> depModels;
        std::map<std::string, double> state;
        std::map<std::string, double> stateInit;
        std::map<std::string, double>* gState;

        // Function(s)
        virtual void update(double timeEval) = 0; // Pure virtual
        virtual void update_gState() = 0;

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

        void update_deps()
        {
            for (const auto& dep : depModels)
            {
                dep->update(state["time"]);
            }
        }

        void set_gState(std::map<std::string, double>* statePtr)
        {
            
            gState = statePtr;

            for (const auto& dep : depModels)
            {
                dep->set_gState(gState);
            }

        }

};

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
        void update_gState() override;

        ~Engine(); // Destructor

};

//---------------------------------------------------------------------------//

class Geodetic : public Model
{
    public:

        // Data

        // Function(s)
        void initialize(double phi);
        void update(double timeEval) override;
        void update_gState() override;

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
        void update(double timeEval) override;
        void update_gState() override;
        void test();
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