// System libraries
#include <map>
#include <cstdio>

// External libraries
#include "gsl/ode-initval2/gsl_odeiv2.h"
#include "gsl/err/gsl_errno.h"

// Project headers
#include "model.h"

//---------------------------------------------------------------------------//

int ode_update(double t, const double y[], double f[], void *params)
{
    
    Model* obj = static_cast<Model*>(params);

    stateMap* state = obj->state;
    //printf("test3\n");
    //---------------------------------//
    *state->at("linPosZ") = y[0];
    *state->at("linVelZ") = y[1];
    //---------------------------------//

    obj->update_deps();

    f[0] = y[1];
    f[1] = *state->at("linAccZ");
    
    return GSL_SUCCESS;
}

/*
THINK!!!

I can't pass a class member as a function pointer to odeSys.function

Make ode_fun_1dof as a utility function outside of the class
*/

//---------------------------------------------------------------------------//

void Flight::init()
{

    time     = 0.0;
    massBody = 2.0;
    
    state = new stateMap;
    init_state(state);

    //-----------------------------------------------------------------------//

    odeSys.function  = &ode_update;
    odeSys.jacobian  = nullptr;
    odeSys.dimension = odeDim;
    odeSys.params    = this;

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

    // time already initialized
    const double tf = 20.0; // time final
    const double dt = 0.1; // output time step
    int nStep = static_cast<int>(tf/dt);
    //printf("test1\n");
    double y[2] = {*state->at("linVelZ"),
                   *state->at("linAccZ")};

    int i;

    for (i = 1; i<=nStep; i++)
    {
        double ti = i*dt;
        //printf("test2\n");
        int status = gsl_odeiv2_driver_apply(odeDriver, state->at("time"), ti, y);

        printf("@t=%f: %f, %f, %f\n", ti, *state->at("linAccZ"), y[1], y[0]);

        /*
        if (status != GSL_SUCCESS)
        {
          printf ("error, return value=%d\n", status);
          break;
        }
        */
    }

}

//---------------------------------------------------------------------------//

Flight::~Flight()
{
    delete state;
    gsl_odeiv2_driver_free(odeDriver);
}