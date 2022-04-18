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

void Engine::init(numpyArray& timeInit  , 
                  numpyArray& thrustInit, 
                  numpyArray& massInit  ) 
{
    
    py::buffer_info timeBuff   = timeInit.request();
    py::buffer_info thrustBuff = thrustInit.request();
    py::buffer_info massBuff   = massInit.request();

    const size_t nTime = timeBuff.size;

    if (timeBuff.ndim != 1 || thrustBuff.ndim != 1 || massBuff.ndim != 1)
    {
        throw std::runtime_error("Input arrays must be 1-D");
    }

    if (thrustBuff.size != nTime || massBuff.size != nTime)
    {
        throw std::runtime_error("Input arrays must have identical lengths");
    }

    double* timeData   = (double*) timeBuff.ptr;
    double* thrustData = (double*) thrustBuff.ptr;
    double* massData   = (double*) massBuff.ptr;

    interp1d_init(thrustSpline, timeData, thrustData, nTime, timeAcc);
    interp1d_init(massSpline  , timeData, massData  , nTime, timeAcc);

    thrust  = interp1d_eval(thrustSpline, 0.0, timeAcc);
    massEng = interp1d_eval(massSpline  , 0.0, timeAcc);

    timeMax = timeData[nTime-1];

    isInit = true;

}

//---------------------------------------------------------------------------//

void Engine::set_state()
{

    state->emplace("thrust" , &thrust);
    state->emplace("massEng", &massEng);
    state->emplace("isBurnout", &isBurnout);

}

//---------------------------------------------------------------------------//

void Engine::update()
{

    update_deps();

    double time = *state->at("time");

    if (!isBurnout)
    {
        
        thrust  = interp1d_eval(thrustSpline, time, timeAcc);
        massEng = interp1d_eval(massSpline  , time, timeAcc);

        if (time >= timeMax)
        {
            isBurnout = 1.0;
        }

    }
    else
    {
        thrust = 0.0;
    }

}

//---------------------------------------------------------------------------//

Engine::~Engine()
{
    
    if (isInit)
    {
        
        gsl_spline_free(thrustSpline);
        gsl_spline_free(massSpline);

        gsl_interp_accel_free(timeAcc);

    }

}