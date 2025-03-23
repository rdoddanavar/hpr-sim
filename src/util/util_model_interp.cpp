// System libraries
#include <cmath>
#include <algorithm>

// External libraries
#include "gsl/interpolation/gsl_interp.h"
#include "gsl/interpolation/gsl_interp2d.h"
#include "gsl/interpolation/gsl_spline.h"
#include "gsl/interpolation/gsl_spline2d.h"

// Project headers
#include "util_model.h"
#include "interp.h"

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
                   const size_t      ny     )
{

    spline = gsl_spline2d_alloc(gsl_interp2d_bilinear, nx, ny);

    gsl_spline2d_init(spline, x, y, z, nx, ny);

}

//---------------------------------------------------------------------------//

// 2-D interpolant initialization - BILINEAR

void interp2d_init(gsl_spline2d*     &spline,
                   const double      x[]    ,
                   const double      y[]    ,
                   const double      z[]    ,
                   const size_t      nx     ,
                   const size_t      ny     ,
                   gsl_interp_accel* &xacc  ,
                   gsl_interp_accel* &yacc  )
{

    spline = gsl_spline2d_alloc(gsl_interp2d_bilinear, nx, ny);
    xacc   = gsl_interp_accel_alloc();
    yacc   = gsl_interp_accel_alloc();

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

//----------------------------------------------------------------------------//

void Interp::init(std::vector<std::vector<double>> dataInd, std::vector<double> dataDep, interpMethod method)
{

    dataInd_ = dataInd;
    dataDep_ = dataDep;
    method_  = method;

    nDim_ = dataInd_.size();

    xMin_ = std::vector<double>(nDim_, 0.0);
    xMax_ = std::vector<double>(nDim_, 0.0);

    iSearch_ = std::vector<size_t>(nDim_, 0);

    for (size_t iDim=0; iDim<nDim_; iDim++)
    {
        size_t iLo = 0;
        size_t iHi = dataInd_[iDim].size() - 1;
        iSearch_[iDim]  = floor((iLo + iHi)/2);
    }

    // TODO: error catching for dimension/method mismatch

    switch (method_)
    {
        case LINEAR:
            init_linear();
            break;
        case PCHIP:
            //init_pchip();
            break;
        case BILINEAR:
            //init_bilinear();
            break;
    }

}

void Interp::init_linear()
{
    xMin_[0] = *std::min_element(dataInd_[0].begin(), dataInd_[0].end());
    xMax_[0] = *std::max_element(dataInd_[0].begin(), dataInd_[0].end());
}

double Interp::update(double xq)
{
    double yq;
    switch (method_)
    {
        case LINEAR:
            yq = update_linear(xq);
            break;
        case PCHIP:
            //yq = update_pchip(xq[0]);
            break;
    }
    return yq;
}

double Interp::update(std::vector<double> xq)
{
    double yq;
    switch (method_)
    {
        case BILINEAR:
            //yq = update_bilinear(xq);
            break;
    }
    return yq;
}

double Interp::update_linear(double xq)
{

    const size_t iDim = 0;
    double yq;

    if (xq <= xMin_[iDim]) // No extrapolation, use min y value
    {
        yq = dataDep_[0];
    }
    else if (xq >= xMax_[iDim]) // No extrapolation, use max y value
    {
        yq = dataDep_.back();
    }
    else // Evaluate on valid interval
    {

        search(iDim, xq);

        double x0 = dataInd_[iDim][iSearch_[iDim]+0];
        double x1 = dataInd_[iDim][iSearch_[iDim]+1];
        double y0 = dataDep_      [iSearch_[iDim]+0];
        double y1 = dataDep_      [iSearch_[iDim]+1];

        double dx = x1 - x0;
        double dy = y1 - y0;

        yq = y0 + (dy/dx)*(xq - x0);

    }

    return yq;

}

void Interp::search(size_t iDim, double xq)
{

    // Performs a binary search to find 
    // the index of the first point in x
    // that satisfies x[i] <= xq;
    // Additional logic is used to accelerate the search
    // using edge cases or the previous result

    const std::vector<double>& x = dataInd_[iDim];
    size_t& iSearch = iSearch_[iDim];

    size_t iLo = 0;
    size_t iHi = x.size() - 1;

    // Check bounds

    if (xq == x[iLo])
    {
        iSearch = iLo;
        return;
    }
    else if (xq == x[iHi])
    {
        iSearch = iHi;
        return;
    }

    // Check immediately before and after current index

    if ((iSearch > iLo) && (xq >= x[iSearch-1]) && (xq < x[iSearch]))
    {
        iSearch -= 1;
        return;
    }
    else if ((iSearch < (iHi-1)) && (xq >= x[iSearch+1]) && (xq < x[iSearch+2]))
    {
        iSearch += 1;
        return;
    }

    // Finally, perform binary search

    while (true)
    {

        // xq < x[iLo+1]
        // goal: find index of (floor) closest x point)

        if ((xq >= x[iSearch]) && (xq < x[iSearch+1]))
            return;
        else
        {
            if (xq < x[iSearch])
            {
                iHi = iSearch;
            }
            else if (xq > x[iSearch])
            {
                iLo = iSearch;
            }
            iSearch = floor((iLo + iHi)/2);
        }

    }

}

/*

void Interp::init_pchip()
{
    ;
}

void Interp::update_phip()
{
    ;
}

void Interp::init_bilinear()
{
    ;
}

void Interp::update_bilinear()
{
    ;
}
*/


