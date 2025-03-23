#pragma once

// System libraries
#include <cstdio>
#include <string>
#include <vector>
#include <set>

// External libraries
#include "gsl/interpolation/gsl_interp.h"
#include "gsl/interpolation/gsl_spline.h"
#include "gsl/interpolation/gsl_spline2d.h"
#include "gsl/ode-initval2/gsl_odeiv2.h"
#include "eigen/Eigen/Core"

// Project headers
#include "telem.h"
#include "odeint.h"
#include "interp.h"

//---------------------------------------------------------------------------//

// Macros

// Namespaces

// Type aliases

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

        void init_state(Telem* telemIn)
        {
            telem = telemIn;
            set_state(&(telem->state_));
        }

        void set_state(stateMap* stateIn)
        {

            state = stateIn;
            set_state_fields();

            for (const auto& dep : depModels)
            {
                dep->set_state(state);
            }

        }

        bool isInit_ = false;
        Telem* telem = nullptr;
        stateMap* state = nullptr;
        std::set<Model*> depModels; // std::set enforces unique elements

        // TODO: track which state fields are necessary to satisfy model

};

//---------------------------------------------------------------------------//

class Test : public Model
{

    public:

        void init(std::vector<std::string> stateFields_);
        void set_state_fields() override;
        void update() override;

        void   set_state_data(std::string field, double data);
        double get_state_data(std::string field); // TODO: make getter function const

    private:

        std::vector<std::string> stateFields_;
        std::vector<double>      stateData_;


};

//---------------------------------------------------------------------------//

class Engine : public Model
{

    public:

        void init(numpyArray& timeArray  ,
                  numpyArray& thrustArray,
                  numpyArray& massArray  );

        void set_state_fields() override;
        void update() override;

    private:

        // State variables
        double thrust_ {0.0};
        double massEng_ {0.0};
        double isBurnout_ {0.0};

        // Miscellaneous
        double timeMax_ {0.0};

        Interp thrustInterp_;
        Interp massInterp_;

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

        void init(double timeStep, std::string termField, std::string termLogic, double termValue);
        void set_state_fields() override;
        void update() override;

        ~Flight();

        OdeSolver odeSolver_; // ODE solver settings & driver

    private:

        // State variables
        double time_ {0.0};

        // Miscellaneous
        double timeStep_ {0.0};
        bool   flightTerm_ {false};

        // TODO: create "phase" structure to capture all flags

        std::string termField_ {""};
        double      termValue_ {0.0};

        typedef bool (Flight::*TermEvalFun)();
        TermEvalFun termEval_ {nullptr};

        bool term_eval_less    () {return *state->at(termField_) <  termValue_;}; 
        bool term_eval_leq     () {return *state->at(termField_) <= termValue_;}; 
        bool term_eval_equal   () {return *state->at(termField_) == termValue_;}; 
        bool term_eval_geq     () {return *state->at(termField_) >= termValue_;}; 
        bool term_eval_greater () {return *state->at(termField_) >  termValue_;}; 

        std::unordered_map<std::string, TermEvalFun> termEvalMap_ = 
        {
            {"less"   , &Flight::term_eval_less   },
            {"leq"    , &Flight::term_eval_leq    },
            {"equal"  , &Flight::term_eval_equal  },
            {"geq"    , &Flight::term_eval_geq    },
            {"greater", &Flight::term_eval_greater},
        };

};
