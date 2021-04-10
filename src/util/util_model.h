#pragma once

// System libraries
#include <map>
#include <string>

// External libraries
#include "gsl/interpolation/gsl_interp.h"
#include "gsl/interpolation/gsl_spline.h"
#include "gsl/ode-initval2/gsl_odeiv2.h"

// Project headers
// <none>

//---------------------------------------------------------------------------//

void interp1d_init(const double      x[]    ,
                   const double      y[]    ,
                   const size_t      n      ,
                   gsl_spline*       &spline,
                   gsl_interp_accel* &acc   );

//---------------------------------------------------------------------------//

double interp1d_eval(gsl_spline*       spline,
                     const double      xq    ,
                     gsl_interp_accel* acc   );

//---------------------------------------------------------------------------//

const std::map<std::string, const gsl_odeiv2_step_type*> odeMethod = 
{
    
    {"rk2"  , gsl_odeiv2_step_rk2  },
    {"rk4"  , gsl_odeiv2_step_rk4  },
    {"rkf45", gsl_odeiv2_step_rkf45},
    {"rkck" , gsl_odeiv2_step_rkck },
    {"rk8pd", gsl_odeiv2_step_rk8pd},
    
};

//---------------------------------------------------------------------------//

struct OdeSolver
{
    
    gsl_odeiv2_system  sys;
    gsl_odeiv2_driver* driver;

    size_t      dim;
    std::string method;
    double      hStart;
    double      epsAbs;
    double      epsRel;

};