#pragma once

// System libraries
#include <map>

// External libraries
#include "gsl/ode-initval2/gsl_odeiv2.h"

// Project headers
// <none>

//---------------------------------------------------------------------------//

const std::map<std::string, const gsl_odeiv2_step_type*> method = 
{
    {"rk2"  , gsl_odeiv2_step_rk2  },
    {"rk4"  , gsl_odeiv2_step_rk4  },
    {"rkf45", gsl_odeiv2_step_rkf45},
    {"rkck" , gsl_odeiv2_step_rkck },
    {"rk8pd", gsl_odeiv2_step_rk8pd},
};

//---------------------------------------------------------------------------//

struct Solver
{
    
    gsl_odeiv2_system sys;
    gsl_odeiv2_driver* driver;

    size_t dim;

    double hStart;
    double epsAbs;
    double epsRel;

};