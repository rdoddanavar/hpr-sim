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
    *state->at("time")  = t;

    *state->at("x" ) = y[ 0];
    *state->at("y" ) = y[ 1];
    *state->at("z" ) = y[ 2];
    *state->at("u" ) = y[ 3];
    *state->at("v" ) = y[ 4];
    *state->at("w" ) = y[ 5];
    *state->at("p" ) = y[ 6];
    *state->at("q" ) = y[ 7];
    *state->at("r" ) = y[ 8];
    *state->at("q0") = y[ 9];
    *state->at("q1") = y[10]; 
    *state->at("q2") = y[11];
    *state->at("q3") = y[12];

    flight->update_deps();

    // Set state derivatives for solver
    f[ 0] = *state->at("xDot" );
    f[ 1] = *state->at("yDot" );
    f[ 2] = *state->at("zDot" );
    f[ 3] = *state->at("uDot" );
    f[ 4] = *state->at("vDot" );
    f[ 5] = *state->at("wDot" );
    f[ 6] = *state->at("pDot" );
    f[ 7] = *state->at("qDot" );
    f[ 8] = *state->at("rDot" );
    f[ 9] = *state->at("q0Dot");
    f[10] = *state->at("q1Dot");
    f[11] = *state->at("q2Dot");
    f[12] = *state->at("q3Dot");

}

void Flight::update()
{

    // Initialize state
    update_deps();
    telem->update();

    // Solve ODE system
    odeInt_.y_[ 0] = *state->at("x" );
    odeInt_.y_[ 1] = *state->at("y" );
    odeInt_.y_[ 2] = *state->at("z" );
    odeInt_.y_[ 3] = *state->at("u" );
    odeInt_.y_[ 4] = *state->at("v" );
    odeInt_.y_[ 5] = *state->at("w" );
    odeInt_.y_[ 6] = *state->at("p" );
    odeInt_.y_[ 7] = *state->at("q" );
    odeInt_.y_[ 8] = *state->at("r" );
    odeInt_.y_[ 9] = *state->at("q0");
    odeInt_.y_[10] = *state->at("q1");
    odeInt_.y_[11] = *state->at("q2");
    odeInt_.y_[12] = *state->at("q3");

    while (!flightTerm_)
    {

        time_ = odeInt_.update();

        *state->at("x" ) = odeInt_.y_[ 0];
        *state->at("y" ) = odeInt_.y_[ 1];
        *state->at("z" ) = odeInt_.y_[ 2];
        *state->at("u" ) = odeInt_.y_[ 3];
        *state->at("v" ) = odeInt_.y_[ 4];
        *state->at("w" ) = odeInt_.y_[ 5];
        *state->at("p" ) = odeInt_.y_[ 6];
        *state->at("q" ) = odeInt_.y_[ 7];
        *state->at("r" ) = odeInt_.y_[ 8];
        *state->at("q0") = odeInt_.y_[ 9];
        *state->at("q1") = odeInt_.y_[10]; 
        *state->at("q2") = odeInt_.y_[11];
        *state->at("q3") = odeInt_.y_[12];

        update_deps(); // Reset state to correct time step

        telem->update();

        flightTerm_ = (this->*termEval_)(); // TODO: chain logic with apogeeFlag

        // TODO: exit condition for nan, or if termEval is never satisfied

    }
}
