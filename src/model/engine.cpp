// System libraries
// <none>

// External libraries
#include "gsl/interpolation/gsl_interp.h"
#include "gsl/interpolation/gsl_spline.h"
#include "pybind11/numpy.h"

// Project headers
#include "util_interp.h"
#include "model.h"

//---------------------------------------------------------------------------//

void Engine::init(py::array_t<double> timeInit  , 
                  py::array_t<double> thrustInit, 
                  py::array_t<double> massInit  ) 
{

    auto timeBuff   = timeInit.request();
    auto thrustBuff = thrustInit.request();
    auto massBuff   = massInit.request();

    double* timeData   = (double*) timeBuff.ptr;
    double* thrustData = (double*) thrustBuff.ptr;
    double* massData   = (double*) massBuff.ptr;

    const size_t n = timeBuff.size;

    interp1d_init(timeData, thrustData, n, thrustSpline, thrustAcc);
    interp1d_init(timeData, massData  , n, massSpline  , massAcc  );

    thrust = interp1d_eval(thrustSpline, 0.0, thrustAcc);
    mass   = interp1d_eval(massSpline  , 0.0, massAcc  );

    isInit = true;

}

//---------------------------------------------------------------------------//

void Engine::set_state()
{

    state->emplace("thrust" , &thrust);
    state->emplace("massEng", &mass  );

}

//---------------------------------------------------------------------------//

void Engine::update()
{

    update_deps();

    double time = *state->at("time");

    thrust = interp1d_eval(thrustSpline, time, thrustAcc);
    mass   = interp1d_eval(massSpline  , time, massAcc  );

}

//---------------------------------------------------------------------------//

Engine::~Engine()
{
    
    gsl_spline_free(thrustSpline);
    gsl_spline_free(massSpline);

    gsl_interp_accel_free(thrustAcc);
    gsl_interp_accel_free(massAcc);

}