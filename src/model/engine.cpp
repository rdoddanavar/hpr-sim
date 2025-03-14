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

    indvec tmp1 = {std::vector<double>(timeData, timeData + timeBuff.size)};
    depvec tmp2 = std::vector<double>(thrustData, thrustData + thrustBuff.size);
    depvec tmp3 = std::vector<double>(massData, massData + massBuff.size);

    thrustInterp.init(tmp1, tmp2, LINEAR);
    massInterp.init(tmp1, tmp3, LINEAR);    

    thrust  = thrustInterp.update(0.0);
    massEng = massInterp.update(0.0);

    timeMax = timeData[nTime-1];

    isInit = true;

}

//---------------------------------------------------------------------------//

void Engine::set_state_fields()
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

        thrust  = thrustInterp.update(time);
        massEng = massInterp.update(time);

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
