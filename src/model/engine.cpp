// header

// System libraries
#include <cstdio>

// External libraries
#include "gsl/interpolation/gsl_interp.h"
#include "gsl/interpolation/gsl_spline.h"
#include "pybind11/numpy.h"

// Project headers
#include "util_interp.h"
#include "engine.h"

void Engine::initialize(py::array_t<double> time  , 
                        py::array_t<double> thrust, 
                        py::array_t<double> mass  ) 
{

    auto timeBuff   = time.request();
    auto thrustBuff = thrust.request();
    auto massBuff   = mass.request();

    double* timeData   = (double*) timeBuff.ptr;
    double* thrustData = (double*) thrustBuff.ptr;
    double* massData   = (double*) massBuff.ptr;

    size_t n = timeBuff.size;

    interp1d_init(timeData, thrustData, n, thrustSpline, thrustAcc);
    interp1d_init(timeData, massData  , n, massSpline  , massAcc  );

    stateInit["time"]   = timeData[0];
    stateInit["thrust"] = thrustData[0];
    stateInit["mass"]   = massData[0];

    reset(); // Set state to IC's

}

void Engine::update(double timeEval)
{
    
    //double yq = interp1d_eval(spline, xq, acc);

    state["thrust"] = interp1d_eval(thrustSpline, timeEval, thrustAcc);
    state["mass"]   = interp1d_eval(massSpline  , timeEval, massAcc  );

    //return state["thrust"]

}

Engine::~Engine()
{
    
    gsl_spline_free(thrustSpline);
    gsl_spline_free(massSpline);

    gsl_interp_accel_free(thrustAcc);
    gsl_interp_accel_free(massAcc);

}