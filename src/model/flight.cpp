// System headers
#include <string>

// External headers
// <none>

// Internal headers
#include "model.h"
#include "telem.h"
#include "odeint.h"

//---------------------------------------------------------------------------//

void Flight::init(double timeStep, std::string termField, std::string termLogic, double termValue)
{

    timeStep_  = timeStep;
    termField_ = termField;
    termValue_ = termValue;
    termEval_  = termEvalMap_[termLogic];

    isInit_ = true;

}

//---------------------------------------------------------------------------//

void Flight::set_state_fields()
{
    state->emplace("time", &time_);
}

//---------------------------------------------------------------------------//

void Flight::update()
{

    // Initialize state
    int iStep = 0;
    update_deps();
    telem->update();

    // Solve ODE system
    double y[] = {*state->at("linPosZ"),
                  *state->at("linVelZ")};

    while (!flightTerm_)
    {

        double ti = ++iStep*timeStep_;

        odeint_.update();

        update_deps(); // Reset state to correct time step

        telem->update();

        flightTerm_ = (this->*termEval_)(); // TODO: chain logic with apogeeFlag

        // TODO: exit condition for nan, or if termEval is never satisfied

    }
}
