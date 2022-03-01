// System libraries

// External libraries
#include "gsl/interpolation/gsl_interp.h"
#include "gsl/interpolation/gsl_interp2d.h"
#include "gsl/interpolation/gsl_spline.h"
#include "gsl/interpolation/gsl_spline2d.h"

// Project headers
#include "util_model.h"

//---------------------------------------------------------------------------//

// TODO: enforce correct data array sizes during interp/spline initialization

//---------------------------------------------------------------------------//

// 1-D interpolant initialization - LINEAR

void interp1d_init(gsl_interp*       &interp,
                   const double      x[]    ,
                   const double      y[]    ,
                   const size_t      n      ,
                   gsl_interp_accel* &acc   )
{
    
    interp = gsl_interp_alloc(gsl_interp_linear, n);
    acc    = gsl_interp_accel_alloc();

    gsl_interp_init(interp, x, y, n);

}

//---------------------------------------------------------------------------//

// 1-D interpolant initialization - SPLINE (STEFFEN)

void interp1d_init(gsl_spline*       &spline,
                   const double      x[]    ,
                   const double      y[]    ,
                   const size_t      n      ,
                   gsl_interp_accel* &acc   )
{
    
    spline = gsl_spline_alloc(gsl_interp_steffen, n);
    acc    = gsl_interp_accel_alloc();

    gsl_spline_init(spline, x, y, n);

}

//---------------------------------------------------------------------------//

// 2-D interpolant initialization - BILINEAR

void interp2d_init(gsl_spline2d*     &spline,
                   const double      x[]    ,
                   const double      y[]    ,
                   const double      z[]    ,
                   const size_t      nx     ,
                   const size_t      ny     ,
                   gsl_interp_accel* &acc   )
{

    spline = gsl_spline2d_alloc(gsl_interp2d_bilinear, nx, ny);
    acc    = gsl_interp_accel_alloc();

    gsl_spline2d_init(spline, x, y, z, nx, ny);

}

//---------------------------------------------------------------------------//

// 1-D interpolant evaluation - LINEAR

double interp1d_eval(gsl_interp*       interp,
                     const double      x[]   ,
                     const double      y[]   ,
                     const double      xq    ,
                     gsl_interp_accel* acc   )
{

    double       yq;
    const double xmin = interp->xmin;
    const double xmax = interp->xmax;

    size_t n = interp->size;

    if (xq < xmin) // No extrapolation, use min y value
    {
        yq = y[0];
    }
    else if (xq > xmax) // No extrapolation, use max y value
    {
        yq = y[n-1];
    }
    else // Evaluate on valid interval
    {
        yq = gsl_interp_eval(interp, x, y, xq, acc);
    }
    
    return yq;

}

//---------------------------------------------------------------------------//

// 1-D interpolant evaluation - SPLINE

double interp1d_eval(gsl_spline*       spline,
                     const double      xq    ,
                     gsl_interp_accel* acc   )
{

    double       yq;
    const double xmin = spline->interp->xmin;
    const double xmax = spline->interp->xmax;

    size_t n = spline->size;

    if (xq < xmin) // No extrapolation, use min y value
    {
        yq = spline->y[0];
    }
    else if (xq > xmax) // No extrapolation, use max y value
    {
        yq = spline->y[n-1];
    }
    else // Evaluate on valid interval
    {
        yq = gsl_spline_eval(spline, xq, acc);
    }

    return yq;

}

//---------------------------------------------------------------------------//

// 2-D interpolant evaluation

double interp2d_eval(gsl_spline2d*     spline,
                     double            xq    ,
                     double            yq    ,
                     gsl_interp_accel* xacc  ,
                     gsl_interp_accel* yacc  )
{

    double zq;
    const double xmin = spline->interp_object.xmin;
    const double xmax = spline->interp_object.xmax;
    const double ymin = spline->interp_object.ymin;
    const double ymax = spline->interp_object.ymax;

    // No extrapolation; enforce limits on (x,y) domain

    xq = (xq < xmin) ? xmin : xq;
    xq = (xq > xmax) ? xmax : xq;

    yq = (yq < ymin) ? ymin : yq;
    yq = (yq > ymax) ? ymax : yq;

    zq = gsl_spline2d_eval(spline, xq, yq, xacc, yacc);

    return zq;

}