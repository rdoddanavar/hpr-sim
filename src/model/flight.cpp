// System libraries
#include <map>

// External libraries
#include "gsl/ode-initval2/gsl_odeiv2.h"
#include "gsl/err/gsl_errno.h"

// Project headers
#include "model.h"

//---------------------------------------------------------------------------//

void Flight::init()
{

    time     = 0.0;
    massBody = 2.0;
    
    state = new stateMap;

    //-----------------------------------------------------------------------//
    //odeSys    = new gsl_odeiv2_system(&ode_update, NULL, odeDim, NULL);

    odeSys.function  = &ode_update;
    odeSys.jacobian  = nullptr;
    odeSys.dimension = odeDim;
    odeSys.params    = nullptr;

    odeDriver = gsl_odeiv2_driver_alloc_y_new(&odeSys  ,
                                              odeMethod,
                                              odeHStart,
                                              odeEpsAbs,
                                              odeEpsRel);
    //-----------------------------------------------------------------------//

    isInit = true;

}

//---------------------------------------------------------------------------//

void Flight::set_state()
{
    state->emplace("time", &time);
    state->emplace("massBody", &massBody);
}

//---------------------------------------------------------------------------//

void Flight::update()
{
    ;
}

//---------------------------------------------------------------------------//

int Flight::ode_update(double t, const double y[], double f[], void *params)
{
    return GSL_SUCCESS;
}

/*
THINK!!!

I can't pass a class member as a function pointer to odeSys.function

Make ode_fun_1dof as a utility function outside of the class
*/

//---------------------------------------------------------------------------//

Flight::~Flight()
{
    delete state;
    gsl_odeiv2_driver_free(odeDriver);
}