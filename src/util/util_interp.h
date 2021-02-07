#pragma once

// System libraries

// External libraries
#include "gsl/interpolation/gsl_interp.h"
#include "gsl/interpolation/gsl_spline.h"

// Project headers

//---------------------------------------------------------------------------//

// PROTOTYPES

void interp1d_init(const double      x[]    ,
                   const double      y[]    ,
                   const size_t      n      ,
                   gsl_spline*       &spline,
                   gsl_interp_accel* &acc   );

double interp1d_eval(gsl_spline*       spline,
                     const double      xq    ,
                     gsl_interp_accel* acc   );