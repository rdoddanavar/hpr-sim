// System libraries
#include <string>

// External libraries
#include "gsl/ode-initval2/gsl_odeiv2.h"
#include "gsl/err/gsl_errno.h"

// Project headers
#include "model.h"
#include "telem.h"
#include "util_model.h"

//---------------------------------------------------------------------------//

void Flight::init(double timeStep, std::string termField, std::string termCondition, double termValue)
{

    timeStep_  = timeStep;
    termField_ = termField;
    termValue_ = termValue;
    termEval_  = termEvalMap_[termCondition];

    // Solver setup
    odeSolver_.sys.function  = &ode_update;
    odeSolver_.sys.jacobian  = nullptr;
    odeSolver_.sys.dimension = 2;
    odeSolver_.sys.params    = this;

    odeSolver_.hStart = 1e-6;
    odeSolver_.epsAbs = 1e-9;
    odeSolver_.epsRel = 1e-9;

    odeSolver_.set_method("rkf45");

    odeSolver_.driver = gsl_odeiv2_driver_alloc_y_new(&odeSolver_.sys   ,
                                                      odeSolver_.method,
                                                      odeSolver_.hStart,
                                                      odeSolver_.epsAbs,
                                                      odeSolver_.epsRel);

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

    while ((this->*termEval_)())
    {

        double ti = ++iStep*timeStep_;

        int status = gsl_odeiv2_driver_apply(odeSolver_.driver, &time_, ti, y);

        update_deps(); // Reset state to correct time step

        telem->update();

        // TODO: could include more complex logic with an "apogeeFlag"

        /*
        TODO
        if (status != GSL_SUCCESS)
        {
          printf ("error, return value=%d\n", status);
          break;
        }
        */

    }

    flightTerm_ = true; // TODO: better handling for flight termination

}

//---------------------------------------------------------------------------//

int ode_update(double t, const double y[], double f[], void *params)
{

    Model*    flight = static_cast<Model*>(params); // TODO: use correct cast (dynamic_cast?)
    stateMap* state  = flight->state;

    // Set current state
    *state->at("linPosZ") = y[0];
    *state->at("linVelZ") = y[1];

    flight->update_deps();

    // Set state derivatives for solver
    f[0] = y[1];
    f[1] = *state->at("linAccZ");

    return GSL_SUCCESS;

}

//----------------------------------------------------------------------------//

Flight::~Flight()
{

    if (isInit_)
    {
        gsl_odeiv2_driver_free(odeSolver_.driver);
    }

}
