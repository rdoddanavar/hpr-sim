#include "test_gsl.h"

float interp1(double xq)
{
    
    double x[] = {1.0, 2.0, 3.0, 4.0};
    double y[] = {1.0, 4.0, 9.0, 25.0};

    int n = 4;

    gsl_interp_accel *acc = gsl_interp_accel_alloc();
    gsl_interp *linear = gsl_interp_alloc(gsl_interp_linear, n);
    gsl_interp_init(linear, x, y, n);

    double yq = gsl_interp_eval(linear, x, y, xq, acc);
    
    return yq;
}