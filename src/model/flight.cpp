// System headers
#include <string>

// External headers
// <none>

// Internal headers
#include "model.h"
#include "telem.h"
#include "odeint.h"

//---------------------------------------------------------------------------//

void ode_update(double t, double* y, double* f, void *params);

void Flight::init(double timeStep, std::string termField, std::string termLogic, double termValue)
{

    timeStep_  = timeStep;
    termField_ = termField;
    termValue_ = termValue;
    termEval_  = termEvalMap_[termLogic];

    odeInt_.init(timeStep_, &ode_update, this);

    isInit_ = true;

}

//---------------------------------------------------------------------------//

void Flight::set_state_fields()
{
    state->emplace("time", &time_);
}

//---------------------------------------------------------------------------//

void ode_update(double t, double* y, double* f, void *params)
{

    Model*    flight = static_cast<Model*>(params);
    stateMap* state  = flight->state;

    // Set current state
    *state->at("time") = t;

    *state->at("linPosXE") = y[ 0];
    *state->at("linPosYE") = y[ 1];
    *state->at("linPosZE") = y[ 2];
    *state->at("linVelXB") = y[ 3];
    *state->at("linVelYB") = y[ 4];
    *state->at("linVelZB") = y[ 5];
    *state->at("angVelXB") = y[ 6];
    *state->at("angVelYB") = y[ 7];
    *state->at("angVelZB") = y[ 8];
    *state->at("q0"      ) = y[ 9];
    *state->at("q1"      ) = y[10]; 
    *state->at("q2"      ) = y[11];
    *state->at("q3"      ) = y[12];

    // Update models to get state derivatives
    flight->update_deps();

    // Set state derivatives for solver
    f[ 0] = *state->at("linVelXE");
    f[ 1] = *state->at("linVelYE");
    f[ 2] = *state->at("linVelZE");
    f[ 3] = *state->at("linAccXB");
    f[ 4] = *state->at("linAccYB");
    f[ 5] = *state->at("linAccZB");
    f[ 6] = *state->at("angAccXB");
    f[ 7] = *state->at("angAccYB");
    f[ 8] = *state->at("angAccZB");
    f[ 9] = *state->at("q0Dot"   );
    f[10] = *state->at("q1Dot"   );
    f[11] = *state->at("q2Dot"   );
    f[12] = *state->at("q3Dot"   );

}

void Flight::update()
{

    // Initialize state
    update_deps();
    telem->update();

    // Initialize ODE solver
    odeInt_.y_[ 0] = *state->at("linPosXE");
    odeInt_.y_[ 1] = *state->at("linPosYE");
    odeInt_.y_[ 2] = *state->at("linPosZE");
    odeInt_.y_[ 3] = *state->at("linVelXB");
    odeInt_.y_[ 4] = *state->at("linVelYB");
    odeInt_.y_[ 5] = *state->at("linVelZB");
    odeInt_.y_[ 6] = *state->at("angVelXB");
    odeInt_.y_[ 7] = *state->at("angVelYB");
    odeInt_.y_[ 8] = *state->at("angVelZB");
    odeInt_.y_[ 9] = *state->at("q0"      );
    odeInt_.y_[10] = *state->at("q1"      );
    odeInt_.y_[11] = *state->at("q2"      );
    odeInt_.y_[12] = *state->at("q3"      );

    while (!flightTerm_)
    {

        // Run ODE soler
        time_ = odeInt_.update();

        // Gather ODE outputs
        *state->at("linPosXE") = odeInt_.y_( 0);
        *state->at("linPosYE") = odeInt_.y_( 1);
        *state->at("linPosZE") = odeInt_.y_( 2);
        *state->at("linVelXB") = odeInt_.y_( 3);
        *state->at("linVelYB") = odeInt_.y_( 4);
        *state->at("linVelZB") = odeInt_.y_( 5);
        *state->at("angVelXB") = odeInt_.y_( 6);
        *state->at("angVelYB") = odeInt_.y_( 7);
        *state->at("angVelZB") = odeInt_.y_( 8);
        *state->at("q0"      ) = odeInt_.y_( 9);
        *state->at("q1"      ) = odeInt_.y_(10); 
        *state->at("q2"      ) = odeInt_.y_(11);
        *state->at("q3"      ) = odeInt_.y_(12);

        // Reset state to correct time step
        update_deps();

        // Update telemetry logger
        telem->update();

        // Evaluate flight termination criteria
        // TODO: exit condition for nan, or if termEval is never satisfied
        flightTerm_ = (this->*termEval_)() || telem->is_nan(); // TODO: chain logic with apogeeFlag

    }
}
