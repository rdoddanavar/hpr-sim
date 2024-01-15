#pragma once

// System libraries
#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>

// External libraries
#include "pybind11/pybind11.h"
#include "pybind11/numpy.h"
#include "gsl/interpolation/gsl_interp.h"
#include "gsl/interpolation/gsl_spline.h"
#include "gsl/interpolation/gsl_spline2d.h"
#include "gsl/ode-initval2/gsl_odeiv2.h"
#include "eigen/Eigen/Core"

// Project headers
#include "util_model.h"

//---------------------------------------------------------------------------//

// Namespaces
namespace py = pybind11;

// Type aliases
using stateMap    = std::map<std::string, double*>;
using stateMapPtr = std::shared_ptr<stateMap>;
using stateMapVec = std::map<std::string, std::vector<double>>;
using numpyArray  = py::array_t<double, py::array::c_style | py::array::forcecast>;

// TODO: consider replacing std::map w/ std::unordered_map for performance
// Performance vs. memory usage?

//---------------------------------------------------------------------------//

class Model
{

    public:

        // Function(s)
        virtual void set_state() = 0; // Pure virtual
        virtual void update()    = 0; // Pure virtual

        void add_dep(Model* dep)
        {
            depModels.insert(dep);
        }

        void add_dep(std::vector<Model*> depList)
        {
            for (const auto& dep : depList)
            {
                depModels.insert(dep); // Quietly rejects duplicates
            }
        }

        void update_deps()
        {
            for (const auto& dep : depModels)
            {
                dep->update();
            }
        }

        void init_state()
        {
            init_state(stateMapPtr(new stateMap));
        }

        void init_state(stateMapPtr stateIn)
        {

            state = stateIn;
            set_state();

            for (const auto& dep : depModels)
            {
                dep->init_state(state);
            }

        }

        bool isInit = false;
        stateMapPtr state;
        std::set<Model*> depModels; // std::set enforces unique elements

        // TODO: track which state fields are necessary to satisfy model

};

//---------------------------------------------------------------------------//

class Test : public Model
{

    public:

        void init(std::vector<std::string> stateFieldsInit);
        void set_state() override;
        void update() override;

        void   set_state_data(std::string field, double data);
        double get_state_data(std::string field); // TODO: make getter function const

    private:

        std::vector<std::string> stateFields;
        std::vector<double>      stateData;


};

//---------------------------------------------------------------------------//

class Engine : public Model
{

    public:

        void init(numpyArray& timeInit  , 
                  numpyArray& thrustInit, 
                  numpyArray& massInit  );

        void set_state() override;
        void update() override;

        ~Engine(); // Destructor

    private:

        // State variables
        double thrust;
        double massEng;
        double isBurnout;

        // Miscellaneous
        gsl_spline* thrustSpline;
        gsl_spline* massSpline;

        gsl_interp_accel* timeAcc;
        
        double timeMax; 

};

//---------------------------------------------------------------------------//

class Mass : public Model
{

    public:

        void init(double massBodyInit);
        void set_state() override;
        void update() override;

    private:

        // State variables
        double mass;

        // Miscellaneous
        double massBody;

};


//---------------------------------------------------------------------------//

class Geodetic : public Model
{
    public:

        void init(double phiInit, double altInit);
        void set_state() override;
        void update() override;

    private:

        // Model subroutines
        void wgs84_init();
        void wgs84();

        // State variables
        double altitudeMSL; // [m]
        double altitudeAGL; // [m]
        double altitudeGP;  // [m]
        double gravity;     // [m/s^2]
        double gravity0;    // [m/s^2]

        // Miscellaneous
        double a2;           // [m^2]
        double phi;          // [rad]
        double sin2phi;      // [-]
        double radiusE;      // [m]
        double altitudeMSL0; // [m]
        double gamma;        // [m/s^2]

};

//---------------------------------------------------------------------------//

class Atmosphere : public Model
{

    public:

        void init(double tempInit, double pressInit);
        void set_state() override;
        void update() override;

    private:

        // Model subroutines
        void usStd1976_init(double altitudeMSL0);
        void usStd1976(double altitudeMSL);
        void sutherland();

        // State variables
        double temperature;      // [K]
        double pressure;         // [Pa]
        double density;          // [kg/m^3]
        double speedSound;       // [m/s]
        double dynamicViscosity; // [-]

        // Miscellaneous
        double gravity0; // [m/s^2]

        std::vector<double> profileAlt;   // [m]
        std::vector<double> profileTemp;  // [K]
        std::vector<double> profilePress; // [Pa]

};

//---------------------------------------------------------------------------//

class Aerodynamics : public Model
{
    
    public:
        
        void init(double      refAreaInit   ,
                  numpyArray& machInit      , 
                  numpyArray& alphaInit     ,
                  numpyArray& cpTotalInit   ,
                  numpyArray& clPowerOffInit,
                  numpyArray& cdPowerOffInit,
                  numpyArray& clPowerOnInit ,
                  numpyArray& cdPowerOnInit );

        void set_state() override;
        void update() override;

        ~Aerodynamics(); // Destructor

    private:

        // State variables
        double dynamicPressure; // [N/m^2]
        double mach;            // [-]
        double reynolds;        // [-]
        double alphaT;          // [rad]
        double dragCoeff;       // [-]
        double liftCoeff;       // [-]
        double centerPressure;  // [m]
        double dragForce;       // [N]
        double liftForce;       // [N]

        // Miscellaneous
        double refArea; // [m^2]
        
        gsl_spline2d* cpTotalSpline;
        gsl_spline2d* clPowerOffSpline;
        gsl_spline2d* cdPowerOffSpline;
        gsl_spline2d* clPowerOnSpline;
        gsl_spline2d* cdPowerOnSpline;

        gsl_interp_accel* machAcc;
        gsl_interp_accel* alphaAcc;

        // Eigen::Vector3d forceAero;  // Force  [N]
        // Eigen::Vector3d momentAero; // Moment [N*m]

};

//---------------------------------------------------------------------------//

class EOM : public Model
{

    public:

        void init(double launchAzInit, double launchElInit);
        void set_state() override;
        void update() override;

    private:

        // State variables
        Eigen::Vector3d forceB;  // Force                BODY  [N]
        Eigen::Vector3d linAccB; // Linear acceleration  BODY  [m/s^2]
        Eigen::Vector3d linVelB; // Linear velocity      BODY  [m/s]
        Eigen::Vector3d linVelE; // Linear velocity      EARTH [m/s]
        Eigen::Vector3d linPosE; // Linear position      EARTH [m]

        Eigen::Vector3d momentB; // Moment               BODY  [N*m]
        Eigen::Vector3d angAccB; // Angular acceleration BODY  [rad/s^2]
        Eigen::Vector3d angVelB; // Angular velocity     BODY  [rad/s]
        Eigen::Vector3d angVelE; // Angular velocity     EARTH [rad/s]
        Eigen::Vector3d angPosE; // Angular position     EARTH [rad]

        // Miscellanenous
        double launchAz; // Launch rail azimuth   [rad]
        double launchEl; // Launch rail elevation [rad]

        bool launchFlag = false;

};

//---------------------------------------------------------------------------//

int ode_update(double t, const double y[], double f[], void *params);

class Flight : public Model
{

    public:

        void init(double tfInit, double dtInit, double t0Init, int nPrecInit);
        void set_state() override;
        void update() override;

        void write_telem(std::string fileOut);
        void write_stats(std::string fileOut);

        static void set_telem(std::vector<std::string> telemFieldsInit);

        ~Flight(); // Destructor

        static std::vector<std::string> telemFieldsDefault;
        static std::vector<std::string> telemUnitsDefault;

        static std::vector<std::string> telemFields;
        static std::vector<std::string> telemUnits;

        OdeSolver odeSolver; // ODE solver settings & driver

    private:

        // State variables
        stateMapVec stateTelem;
        double time;

        // Miscellaneous
        double t0;
        double dt;
        double tf;
        int    nStep;
        int    nPrec;

        double flightTime;
        bool   flightTerm = false;

        // TODO: create "phase" structure to capture all flags

};
