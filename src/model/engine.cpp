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

    stateInit["time"]    = timeData[0];
    stateInit["thrust"]  = thrustData[0];
    stateInit["engMass"] = massData[0];

    reset(); // Set state to IC's

    isInit = true;

}

//---------------------------------------------------------------------------//

void Engine::update(stateMap& gState)
{

    update_deps(gState);

    double timeEval = gState["time"];

    state["time"]    = timeEval;
    state["thrust"]  = interp1d_eval(thrustSpline, timeEval, thrustAcc);
    state["engMass"] = interp1d_eval(massSpline  , timeEval, massAcc  );

    update_gState(gState);

}

//---------------------------------------------------------------------------//

void Engine::update_gState(stateMap& gState)
{
    gState["engMass"] = state["mass"];
    gState["thrust"]  = state["thrust"];
}

//---------------------------------------------------------------------------//

Engine::~Engine()
{
    
    gsl_spline_free(thrustSpline);
    gsl_spline_free(massSpline);

    gsl_interp_accel_free(thrustAcc);
    gsl_interp_accel_free(massAcc);

}