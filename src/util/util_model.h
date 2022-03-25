#pragma once

// System libraries
#include <map>
#include <string>

// External libraries
#include "gsl/interpolation/gsl_interp.h"
#include "gsl/interpolation/gsl_interp2d.h"
#include "gsl/interpolation/gsl_spline.h"
#include "gsl/interpolation/gsl_spline2d.h"
#include "gsl/ode-initval2/gsl_odeiv2.h"

// Project headers
// <none>

//---------------------------------------------------------------------------//

void interp1d_init(gsl_interp*       &interp,
                   const double      x[]    ,
                   const double      y[]    ,
                   const size_t      n      ,
                   gsl_interp_accel* &acc   );

//---------------------------------------------------------------------------//

void interp1d_init(gsl_spline*       &spline,
                   const double      x[]    ,
                   const double      y[]    ,
                   const size_t      n      ,
                   gsl_interp_accel* &acc   );

//---------------------------------------------------------------------------//

void interp2d_init(gsl_spline2d*     &spline,
                   const double      x[]    ,
                   const double      y[]    ,
                   const double      z[]    ,
                   const size_t      nx     ,
                   const size_t      ny     );

//---------------------------------------------------------------------------//

void interp2d_init(gsl_spline2d*     &spline,
                   const double      x[]    ,
                   const double      y[]    ,
                   const double      z[]    ,
                   const size_t      nx     ,
                   const size_t      ny     ,
                   gsl_interp_accel* &xacc  ,
                   gsl_interp_accel* &yacc  );

//---------------------------------------------------------------------------//

double interp1d_eval(gsl_interp*       interp,
                     const double      x[]   ,
                     const double      y[]   ,
                     const double      xq    ,
                     gsl_interp_accel* acc   );

//---------------------------------------------------------------------------//

double interp1d_eval(gsl_spline*       spline,
                     const double      xq    ,
                     gsl_interp_accel* acc   );

//---------------------------------------------------------------------------//

double interp2d_eval(gsl_spline2d*     spline,
                     const double      xq    ,
                     const double      yq    ,
                     gsl_interp_accel* xacc  ,
                     gsl_interp_accel* yacc  );

//---------------------------------------------------------------------------//

const std::map<std::string, const gsl_odeiv2_step_type*> odeMethods = 
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

    size_t dim;
    double hStart;
    double epsAbs;
    double epsRel;

    const gsl_odeiv2_step_type* method;

    void set_method(std::string name)
    {
        method = odeMethods.at(name);
    }

};