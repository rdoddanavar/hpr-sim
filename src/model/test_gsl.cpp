// System libraries
#include <vector>

// Project headers
#include "test_gsl.h"
#include "gsl/interpolation/gsl_interp.h"
#include "gsl/interpolation/gsl_spline.h"


float interp1(double xq)
{
    
    //double x[] = {1.0, 2.0, 3.0, 4.0};
    //double y[] = {1.0, 4.0, 9.0, 25.0};

    std::vector<double> x = {1.0, 2.0, 3.0, 4.0};
    std::vector<double> y = {1.0, 4.0, 9.0, 25.0};

    double *xArr = x.data();
    double *yArr = y.data();

    size_t n = x.size();

    gsl_interp_accel *acc = gsl_interp_accel_alloc();

    /*
    LINEAR INTERP

    gsl_interp *linear = gsl_interp_alloc(gsl_interp_linear, n);
    gsl_interp_init(linear, xArr, yArr, n);

    double yq = gsl_interp_eval(linear, xArr, yArr, xq, acc);
    */

    gsl_spline *spline_steffen = gsl_spline_alloc(gsl_interp_steffen, n);
    gsl_spline_init(spline_steffen, xArr, yArr, n);

    double yq;
    double xMin = spline_steffen->interp->xmin;
    double xMax = spline_steffen->interp->xmax;

    if (xq < xMin)
    {
        yq = spline_steffen->y[0];
    }
    else if (xq > xMax)
    {
        yq = spline_steffen->y[n-1];
    }
    else // Evaluate on valid interval
    {
        yq = gsl_spline_eval(spline_steffen, xq, acc);
    }
    
    gsl_spline_free(spline_steffen);
    gsl_interp_accel_free(acc);

    return yq;
}