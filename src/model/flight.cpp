// System libraries
#include <map>
#include <cstdio>

#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include <iostream>

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

void Flight::init(double t0Init, double dtInit, double tfInit)
{

    time = t0Init;
    dt   = dtInit;
    tf   = tfInit;

    massBody = 10.0;
    
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
    std::vector<std::string> keys = {"time"   ,
                                     "linAccZ",
                                     "linVelZ",
                                     "linPosZ"};

    int nStep = static_cast<int>(tf/dt);

    for (const auto& key : keys)
    {
        stateTelem[key] = std::vector<double>(nStep, 0.0);
    }
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

    int nStep = static_cast<int>(tf/dt);

    double y[2] = {*state->at("linVelZ"),
                   *state->at("linAccZ")};

    int i;

    for (i = 1; i<=nStep; i++)
    {
        double ti = i*dt;

        int status = gsl_odeiv2_driver_apply(odeDriver, state->at("time"), ti, y);

        //-----------------------------------------------------------------------//
        for (const auto& key : keys)
        {
            stateTelem[key][i] = *state->at(key);
        }
        //-----------------------------------------------------------------------//

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

void Flight::write_telem(std::string fileOut) // maybe return bool for success/error status?
{

    // Write header
    std::ostringstream oss;
    const char* delim = ",";

    std::copy(keys.begin(), keys.end() - 1, std::ostream_iterator<std::string>(oss, delim));
    oss << keys.back();

    std::cout << oss.str() << "\n";

    // Write units
    oss.str("");
    oss.clear();

    std::copy(units.begin(), units.end() - 1, std::ostream_iterator<std::string>(oss, delim));
    oss << units.back();

    std::cout << oss.str() << "\n";

    // Write data
    stateMapVec test;
    int nStep = 5;

    for (const auto& key : keys)
    {
        test[key] = std::vector<double>(nStep, 0.0);
    }

    //----------------------//

    for (int i = 0; i < nStep; i++)
    {
        
        oss.str("");
        oss.clear();

        for (const auto& key : keys)
        {
            oss << test[key][i] << delim;
        }

        std::string outStr = oss.str();
        outStr.pop_back();

        std::cout << outStr << "\n";

    }

}

//---------------------------------------------------------------------------//

Flight::~Flight()
{
    
    if (isInit)
    {
        delete state;
        gsl_odeiv2_driver_free(odeDriver);
    }

}