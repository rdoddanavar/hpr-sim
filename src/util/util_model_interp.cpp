// System libraries

// External libraries
#include "gsl/interpolation/gsl_interp.h"
#include "gsl/interpolation/gsl_spline.h"

// Project headers
#include "util_model.h"

//---------------------------------------------------------------------------//

void interp1d_init(const double      x[]    ,
                   const double      y[]    ,
                   const size_t      n      ,
                   gsl_spline*       &spline,
                   gsl_interp_accel* &acc   )
{
    
    acc    = gsl_interp_accel_alloc();
    spline = gsl_spline_alloc(gsl_interp_steffen, n);

    gsl_spline_init(spline, x, y, n);

}

//---------------------------------------------------------------------------//

double interp1d_eval(gsl_spline*       spline,
                     const double      xq    ,
                     gsl_interp_accel* acc   )
{

    double       yq;
    const double xMin = spline->interp->xmin;
    const double xMax = spline->interp->xmax;

    size_t n = spline->size;

    if (xq < xMin) // No extrapolation, use min y value
    {
        yq = spline->y[0];
    }
    else if (xq > xMax) // No extrapolation, use max y value
    {
        yq = spline->y[n-1];
    }
    else // Evaluate on valid interval
    {
        yq = gsl_spline_eval(spline, xq, acc);
    }

    return yq;

}

/*
LINEAR INTERP

gsl_interp *linear = gsl_interp_alloc(gsl_interp_linear, n);
gsl_interp_init(linear, xArr, yArr, n);
gsl_interp_accel *acc = gsl_interp_accel_alloc ();

double yq = gsl_interp_eval(linear, xArr, yArr, xq, acc);

gsl_interp_free(gsl_interp * interp)
gsl_interp_accel_free(gsl_interp_accel *acc)
*/