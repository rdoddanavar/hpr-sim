// header

// System libraries
#include <cstdio>

// Project headers
#include "engine.h"
#include "util_interp.h"
#include "gsl/interpolation/gsl_interp.h"
#include "gsl/interpolation/gsl_spline.h"

void Engine::initialize()
{

    x = {1.0, 2.0, 3.0, 4.0};
    y = {1.0, 4.0, 9.0, 16.0};
        
    double* xArr = x.data();
    double* yArr = y.data();

    size_t n = x.size();

    interp1d_init(xArr, yArr, n, spline, acc);

}

void Engine::update_test(double xq)
{
    
    double yq = interp1d_eval(spline, xq, acc);
    printf("%f\n", yq);

}

// Deconstructor
Engine::~Engine()
{
    gsl_spline_free(spline);
    gsl_interp_accel_free(acc);
}