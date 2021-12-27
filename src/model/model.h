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
#include "gsl/ode-initval2/gsl_odeiv2.h"

// Project headers
#include "util_model.h"

//---------------------------------------------------------------------------//

// Namespaces
namespace py = pybind11;

// Type aliases
using stateMap    = std::map<std::string, double*>;
using stateMapVec = std::map<std::string, std::vector<double>>;

//---------------------------------------------------------------------------//

class Model
{
    
    public: 

        // Data
        bool isInit = false;
        stateMap* state;
        std::set<Model*> depModels; // std::set enforces unique elements

        // Function(s)
        virtual void set_state() = 0; // Pure virtual
        virtual void update()    = 0; // Pure virtual

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
                dep->update();
            }
        }

        void init_state(stateMap* stateIn)
        {
            
            state = stateIn;
            set_state();

            for (const auto& dep : depModels)
            {
                dep->init_state(state);
            }

        }

};

//---------------------------------------------------------------------------//

class Engine : public Model
{
    
    public:

        // Function(s)
        void init(py::array_t<double> timeInit  , 
                  py::array_t<double> thrustInit, 
                  py::array_t<double> massInit  );

        void set_state() override;
        void update() override;

        ~Engine(); // Destructor

    private:

        // Data
        double thrust;
        double massEng;

        gsl_spline *thrustSpline;
        gsl_spline *massSpline;

        gsl_interp_accel* thrustAcc;
        gsl_interp_accel* massAcc;

};

//---------------------------------------------------------------------------//

class Mass : public Model
{
    
    public:

        // Function(s)
        void init(double massBodyInit);
        void set_state() override;
        void update() override;

    private:

        // Data
        double massBody;
        double mass;

};


//---------------------------------------------------------------------------//

class Geodetic : public Model
{
    public:

        // Data

        // Function(s)
        void init(double phiInit);
        void set_state() override;
        void update() override;

    private:

        // Data
        double phi;
        double altitude;
        double gravity;

        // TODO: these vars should be static const
        double gamE;
        double k;
        double e;
        double a;
        double f;
        double m;

        // Function(s)
        double wgs84();

};

//---------------------------------------------------------------------------//

class Atmosphere : public Model
{

    public:
        
        void init(double tempInit, double pressInit, double humInit);
        void set_state() override;
        void update() override;

        ~Atmosphere(); // Destructor

    private:

        double temperature;      // [K]
        double speedSound;       // [m/s]
        double dynamicViscosity; // [-]
        double humidity;         // [-]
        double pressure;         // [Pa]
        double density;          // [kg/m^3]

        double* tempProfileInd; // [m]
        double* tempProfileDep; // [K]

        gsl_interp*       tempInterp;
        gsl_interp_accel* tempAcc;

        void usStd1976_init_temp();
        void usStd1976(double altitude);

};

//---------------------------------------------------------------------------//

class Aerodynamics : public Model
{
    
    public:
        
        void init();
        void set_state() override;
        void update() override;

    private:

        double dynamicPressure; // [N/m^2]
        double mach;            // [-]
        double reynolds;        // [?]

        // double cpX;    // Axial center of pressure [m]
        // double alphaT; // Total angle-of-attack [rad]

        // Eigen::Vector3d forceAero;  // Force  [N]
        // Eigen::Vector3d momentAero; // Moment [N*m]

};

//---------------------------------------------------------------------------//

class EOM : public Model
{

    public:

        void init();
        void set_state() override;
        void update() override;

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

        bool launchFlag = false;

};

//---------------------------------------------------------------------------//

int ode_update(double t, const double y[], double f[], void *params);

class Flight : public Model
{

    public:

        void init(double tfInit, double dtInit, double t0Init);
        void set_state() override;
        void update() override;

        void write_telem(std::string fileOut);
        void write_stats(std::string fileOut);

        static void set_telem(std::vector<std::string> telemFieldsInit);

        static std::vector<std::string> telemFieldsDefault;
        static std::vector<std::string> telemUnitsDefault;

        static std::vector<std::string> telemFields;
        static std::vector<std::string> telemUnits;

        stateMapVec stateTelem;
        OdeSolver   odeSolver; // ODE solver settings & driver

        ~Flight(); // Destructor

    private:

        // Data
        double time;
        double t0;
        double dt;
        double tf;
        int    nStep;
        int    nPrec;

        double flightTime;
        bool   flightTerm = false;

};