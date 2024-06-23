#pragma once

// System libraries
#include <cstdio>
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

// Macros
#define N_TELEM_ARRAY 1000

// Namespaces
namespace py = pybind11;

// Type aliases
using stateMap      = std::unordered_map<std::string, double*>;
using stateMapPtr   = std::shared_ptr<stateMap>;

using telemMap      = std::unordered_map<std::string, double>;
using telemArray    = std::array<double, N_TELEM_ARRAY>;
using telemArrayMap = std::unordered_map<std::string, telemArray>;

using numpyArray    = py::array_t<double, py::array::c_style | py::array::forcecast>;

//---------------------------------------------------------------------------//

class Model
{

    public:

        // Function(s)
        virtual void set_state_fields() = 0; // Pure virtual
        virtual void update()           = 0; // Pure virtual

        void add_deps(std::vector<Model*> depList)
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
            set_state(stateMapPtr(new stateMap));
        }

        void set_state(stateMapPtr stateIn)
        {

            state = stateIn;
            set_state_fields();

            for (const auto& dep : depModels)
            {
                dep->set_state(state);
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
        void set_state_fields() override;
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

        void set_state_fields() override;
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
        void set_state_fields() override;
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
        void set_state_fields() override;
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
        void set_state_fields() override;
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
        
        void init(const double&      refAreaInit  ,
                  const numpyArray& machInit      ,
                  const numpyArray& alphaInit     ,
                  const numpyArray& cpTotalInit   ,
                  const numpyArray& clPowerOffInit,
                  const numpyArray& cdPowerOffInit,
                  const numpyArray& clPowerOnInit ,
                  const numpyArray& cdPowerOnInit );

        void set_state_fields() override;
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

        void init();
        void set_state_fields() override;
        void update() override;

    private:

        // State variables
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

        void init(const std::string& telemModeIn,  const int& nPrecIn, const std::string& outputDir);
        void set_state_fields() override;
        void update() override;

        void set_telem(const std::vector<std::string>& telemFieldsInit, const std::vector<std::string>& telemUnitsInit);
        void write_stats(const std::string& filePath);

        ~Flight(); // Destructor

        static std::vector<std::string> telemFieldsDefault;
        static std::vector<std::string> telemUnitsDefault;

        OdeSolver odeSolver; // ODE solver settings & driver

    private:

        // Telemetry
        void init_telem(const std::string& outputDir);
        void init_telem_text(const std::string& filePath);
        void init_telem_binary(const std::string& filePath);
        void write_telem(const int& iTelem);
        void write_telem_text(const int& iTelem);
        void write_telem_binary(const int& iTelem);
        void telem_interp(const int& iTelem);
        void update_stats(const int& iTelem);

        std::vector<std::string> telemFields;
        std::vector<std::string> telemUnits;

        int nTelemFields;

        telemArrayMap stateTelem;
        telemMap      stateTelemMin;
        telemMap      stateTelemMax;

        std::string   telemMode;
        std::FILE*    telemFile;

        // State variables

        double time;

        // Miscellaneous
        double dt;
        int    nPrec;

        bool   flightTerm{false};
        int    flightTermStep;

        // TODO: create "phase" structure to capture all flags

};
