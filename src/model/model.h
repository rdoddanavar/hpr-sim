#pragma once

// System headers
#include <cstdio>
#include <set>
#include <string>
#include <vector>

// External headers
#include "eigen/Eigen/Core"
#include "eigen/Eigen/Geometry"

// Internal headers
#include "interp.h"
#include "odeint.h"
#include "telem.h"

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

        void init(const numpyArray& timeArray  ,
                  const numpyArray& thrustArray,
                  const numpyArray& massArray  );

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

        void init(double massBody, double bodyLength, double centerGravX, double inertiaX, double inertiaY, double inertiaZ);
        void set_state_fields() override;
        void update() override;

    private:

        // State variables
        double mass_;

        double bodyLength_;

        double centerGravX_;
        double centerGravY_;
        double centerGravZ_;

        double inertiaX_;
        double inertiaY_;
        double inertiaZ_;

        // Miscellaneous
        double massBody_;

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

        void init(const double&     refArea        ,
                  const numpyArray& machArray      ,
                  const numpyArray& alphaArray     ,
                  const numpyArray& cpTotalArray   ,
                  const numpyArray& clPowerOffArray,
                  const numpyArray& cdPowerOffArray,
                  const numpyArray& clPowerOnArray ,
                  const numpyArray& cdPowerOnArray );

        void set_state_fields() override;
        void update() override;

    private:

        // State variables
        double dynamicPressure_; // [N/m^2]
        double mach_;            // [-]
        double reynolds_;        // [-]
        double alphaT_;          // [rad]
        double phiA_;            // [rad]
        double dragCoeff_;       // [-]
        double liftCoeff_;       // [-]
        double dragForce_;       // [N]
        double liftForce_;       // [N]
        double axialForce_;      // [N]
        double normalForce_;     // [N]
        double centerPressure_;  // [m]
        double staticMargin_;    // [-]

        Eigen::Vector3d aeroForce_;  // [N]
        Eigen::Vector3d aeroMoment_; // [N*m]

        // Miscellaneous
        double refDia_;  // [m]
        double refArea_; // [m^2]

        Interp cpTotalInterp_;
        Interp clPowerOffInterp_;
        Interp cdPowerOffInterp_;
        Interp clPowerOnInterp_;
        Interp cdPowerOnInterp_;

        // Eigen::Vector3d forceAero;  // Force  [N]
        // Eigen::Vector3d momentAero; // Moment [N*m]

};

//---------------------------------------------------------------------------//

class EOM : public Model
{

    public:

        void init(double launchAz, double launchEl, double railLength);
        void set_state_fields() override;
        void update() override;

    private:

        // State variables
        Eigen::Vector3d forceB;     // Force                (BODY) [N]
        Eigen::Vector3d linAccB;    // Linear acceleration  (BODY) [m/s^2]
        Eigen::Vector3d linVelB;    // Linear velocity      (BODY) [m/s]
        Eigen::Vector3d linVelE;    // Linear velocity      (ENU)  [m/s]
        Eigen::Vector3d linPosE;    // Linear position      (ENU)  [m]

        Eigen::Vector3d momentB;    // Moment               (BODY) [N*m]
        Eigen::Vector3d angAccB;    // Angular acceleration (BODY) [rad/s^2]
        Eigen::Vector3d angVelB;    // Angular velocity     (BODY) [rad/s]

        Eigen::Vector3d euler;      // Euler angles (321) [rad] 

        Eigen::Quaterniond quat;    // Quaternion ENU to BODY
        Eigen::Vector4d    quatDot; // Quaternion derivative

        double railLength_;
        double railPosInit_;
        bool flagRailExit_ = false;

};

//---------------------------------------------------------------------------//

class Flight : public Model
{

    public:

        void init(double timeStep, std::string termField, std::string termLogic, double termValue);
        void set_state_fields() override;
        void update() override;

        OdeInt<13> odeInt_; // ODE solver

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
