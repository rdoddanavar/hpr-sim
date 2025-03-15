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

void Flight::init(const std::string& solverMethod, const double& solverStep)
{

    // Solver setup
    dt = solverStep;

    odeSolver.sys.function  = &ode_update;
    odeSolver.sys.jacobian  = nullptr;
    odeSolver.sys.dimension = 2;
    odeSolver.sys.params    = this;

    odeSolver.hStart = 1e-6;
    odeSolver.epsAbs = 1e-9;
    odeSolver.epsRel = 1e-9;

    odeSolver.set_method(solverMethod);

    odeSolver.driver = gsl_odeiv2_driver_alloc_y_new(&odeSolver.sys   ,
                                                      odeSolver.method,
                                                      odeSolver.hStart,
                                                      odeSolver.epsAbs,
                                                      odeSolver.epsRel);

    isInit = true;

}

//---------------------------------------------------------------------------//

void Flight::set_state_fields()
{
    state->emplace("time", &time);
}

//---------------------------------------------------------------------------//

void Flight::update()
{

    // Initialize state
    int iStep = 0;
    update_deps();
    telem->init_data();

    // Solve ODE system
    double y[] = {*state->at("linPosZ"),
                  *state->at("linVelZ")};

    while (y[0] >= 0.0)
    {

        double ti = ++iStep*dt;

        int status = gsl_odeiv2_driver_apply(odeSolver.driver, &time, ti, y);

        update_deps(); // Reset state to correct time step

        telem->update(iStep);

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

    flightTerm = true; // TODO: better handling for flight termination
    telem->interp_boundary("linPosZ", 0.0);
    telem->finalize(iStep);

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

    if (isInit)
    {
        gsl_odeiv2_driver_free(odeSolver.driver);
    }

}
