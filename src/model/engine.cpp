// header

// System libraries
#include <cstdio>

// Project headers
#include "util_interp.h"
#include "gsl/interpolation/gsl_interp.h"
#include "gsl/interpolation/gsl_spline.h"
#include "pybind11/numpy.h"

#include "engine.h"

void Engine::initialize(py::array_t<double> time  , 
                        py::array_t<double> thrust, 
                        py::array_t<double> mass  ) 
{

    //x = {1.0, 2.0, 3.0, 4.0};
    //y = {1.0, 4.0, 9.0, 16.0};
        
    //double* xArr = x.data();
    //double* yArr = y.data();

    auto timeBuff   = time.request();
    auto thrustBuff = thrust.request();

    double* xArr = (double*) timeBuff.ptr;
    double* yArr = (double*) thrustBuff.ptr;

    size_t n = timeBuff.size;

    interp1d_init(xArr, yArr, n, spline, acc);

}

void Engine::update(double xq)
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