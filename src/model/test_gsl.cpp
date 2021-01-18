// System libraries
#include <vector>
#include <cstdio>

// Project headers
#include "test_gsl.h"
#include "gsl/interpolation/gsl_interp.h"
#include "gsl/interpolation/gsl_spline.h"

/*
LINEAR INTERP

gsl_interp *linear = gsl_interp_alloc(gsl_interp_linear, n);
gsl_interp_init(linear, xArr, yArr, n);

double yq = gsl_interp_eval(linear, xArr, yArr, xq, acc);
*/


void interp1d_init(const double x[], const double y[], size_t n, gsl_spline* &spline, gsl_interp_accel* &acc)
{
    
    acc    = gsl_interp_accel_alloc();
    spline = gsl_spline_alloc(gsl_interp_steffen, n);

    gsl_spline_init(spline, x, y, n);

}

double interp1d_eval(gsl_spline* spline, double xq, gsl_interp_accel* acc)
{

    double yq;
    double xMin = spline->interp->xmin;
    double xMax = spline->interp->xmax;

    size_t n = spline->size;

    if (xq < xMin)
    {
        yq = spline->y[0];
    }
    else if (xq > xMax)
    {
        yq = spline->y[n-1];
    }
    else // Evaluate on valid interval
    {
        yq = gsl_spline_eval(spline, xq, acc);
    }

    return yq;
}

void Model::initialize()
{

    x = {1.0, 2.0, 3.0, 4.0};
    y = {1.0, 4.0, 9.0, 16.0};
        
    double* xArr = x.data();
    double* yArr = y.data();

    size_t n = x.size();

    interp1d_init(xArr, yArr, n, spline, acc);

}

double Model::update(double xq)
{
    return interp1d_eval(spline, xq, acc);
}

// Deconstructor
Model::~Model()
{
    gsl_spline_free(spline);
    gsl_interp_accel_free(acc);
}
