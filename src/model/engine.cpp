// System libraries
// <none>

// External libraries
#include "pybind11/numpy.h"
#include "gsl/interpolation/gsl_interp.h"
#include "gsl/interpolation/gsl_spline.h"

// Project headers
#include "model.h"
#include "util_model.h"

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

    interp1d_init(thrustSpline, timeData, thrustData, n, thrustAcc);
    interp1d_init(massSpline  , timeData, massData  , n, massAcc  );

    thrust  = interp1d_eval(thrustSpline, 0.0, thrustAcc);
    massEng = interp1d_eval(massSpline  , 0.0, massAcc  );

    isInit = true;

}

//---------------------------------------------------------------------------//

void Engine::set_state()
{

    state->emplace("thrust" , &thrust);
    state->emplace("massEng", &massEng);

}

//---------------------------------------------------------------------------//

void Engine::update()
{

    update_deps();

    double time = *state->at("time");

    thrust  = interp1d_eval(thrustSpline, time, thrustAcc);
    massEng = interp1d_eval(massSpline  , time, massAcc  );

}

//---------------------------------------------------------------------------//

Engine::~Engine()
{
    
    if (isInit)
    {
        
        gsl_spline_free(thrustSpline);
        gsl_spline_free(massSpline);

        gsl_interp_accel_free(thrustAcc);
        gsl_interp_accel_free(massAcc);

    }

}