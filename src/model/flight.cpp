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
    *state->at("time")    = t;
    *state->at("linPosZ") = y[0];
    *state->at("linVelZ") = y[1];

    flight->update_deps();

    // Set state derivatives for solver
    f[0] = y[1];
    f[1] = *state->at("linAccZ");

}

void Flight::update()
{

    // Initialize state
    update_deps();
    telem->update();

    // Solve ODE system
    odeInt_.y_[0] = *state->at("linPosZ");
    odeInt_.y_[1] = *state->at("linVelZ");

    while (!flightTerm_)
    {

        time_ = odeInt_.update();

        *state->at("linPosZ") = odeInt_.y_[0];
        *state->at("linVelZ") = odeInt_.y_[1];

        update_deps(); // Reset state to correct time step

        telem->update();

        flightTerm_ = (this->*termEval_)(); // TODO: chain logic with apogeeFlag

        // TODO: exit condition for nan, or if termEval is never satisfied

    }
}
