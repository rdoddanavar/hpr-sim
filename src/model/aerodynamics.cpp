// System libraries
#include <cmath>

// External libraries
#include "pybind11/numpy.h"
#include "gsl/interpolation/gsl_interp2d.h"
#include "gsl/interpolation/gsl_spline2d.h"

// Project headers
#include "model.h"
#include "util_model.h"

//---------------------------------------------------------------------------//

void Aerodynamics::init(double               refAreaInit   ,
                        py::array_t<double>& machInit      , 
                        py::array_t<double>& alphaInit     ,
                        py::array_t<double>& cdPowerOffInit,
                        py::array_t<double>& cdPowerOnInit ,
                        py::array_t<double>& clPowerOffInit,
                        py::array_t<double>& clPowerOnInit ,
                        py::array_t<double>& cpInit        )
{

    py::buffer_info machBuff       = machInit.request();
    py::buffer_info alphaBuff      = alphaInit.request();
    py::buffer_info cdPowerOffBuff = cdPowerOffInit.request();
    py::buffer_info cdPowerOnBuff  = cdPowerOnInit.request();
    py::buffer_info clPowerOffBuff = clPowerOffInit.request();
    py::buffer_info clPowerOnBuff  = clPowerOnInit.request();
    py::buffer_info cpBuff         = cpInit.request();

    if (machBuff.ndim != 1 || alphaBuff.ndim != 1)
    {
        throw std::runtime_error("Input arrays must be 1-D: mach, alpha");
    }

    if (cdPowerOffBuff.ndim != 2 || cdPowerOnBuff.ndim != 2 ||
        clPowerOffBuff.ndim != 2 || clPowerOnBuff.ndim != 2 ||
        cpBuff.ndim         != 2)
    {
        throw std::runtime_error("Input arrays must be 2-D: cdPowerOff, cdPowerOn, clPowerOff, clPowerOn, cp");
    }

    const size_t nMach  = machBuff.size;
    const size_t nAlpha = alphaBuff.size;
    const size_t nData  = nMach*nAlpha;

    if (cdPowerOffBuff.size != nData || cdPowerOnBuff.size != nData ||
        clPowerOffBuff.size != nData || clPowerOnBuff.size != nData ||
        cpBuff.size         != nData)
    {
        throw std::runtime_error("Input arrays must have a compatible number of elements: cdPowerOff, cdPowerOn, clPowerOff, clPowerOn, cp");
    }

    double* machData       = (double*) machBuff.ptr;
    double* alphaData      = (double*) alphaBuff.ptr;
    double* cdPowerOffData = (double*) cdPowerOffBuff.ptr;
    double* cdPowerOnData  = (double*) cdPowerOnBuff.ptr;
    double* clPowerOffData = (double*) clPowerOffBuff.ptr;
    double* clPowerOnData  = (double*) clPowerOnBuff.ptr;
    double* cpData         = (double*) cpBuff.ptr;

    machAcc  = gsl_interp_accel_alloc();
    alphaAcc = gsl_interp_accel_alloc();

    interp2d_init(cdPowerOffSpline, machData, alphaData, cdPowerOffData, nMach, nAlpha);
    interp2d_init(cdPowerOnSpline,  machData, alphaData, cdPowerOnData,  nMach, nAlpha);
    interp2d_init(clPowerOffSpline, machData, alphaData, clPowerOffData, nMach, nAlpha);
    interp2d_init(clPowerOnSpline,  machData, alphaData, clPowerOnData,  nMach, nAlpha);
    interp2d_init(cpSpline,         machData, alphaData, cpData,         nMach, nAlpha);

    isInit = true;

}

//---------------------------------------------------------------------------//

void Aerodynamics::set_state()
{

    state->emplace("dynamicPressure", &dynamicPressure);
    state->emplace("mach", &mach);
    state->emplace("reynolds", &reynolds);
    state->emplace("alphaT", &alphaT);
    state->emplace("dragCoeff", &dragCoeff);
    state->emplace("liftCoeff", &liftCoeff);
    state->emplace("centerPressure", &centerPressure);

}

//---------------------------------------------------------------------------//

void Aerodynamics::update()
{

    update_deps();

    // Get state data
    double u   = *state->at("linVelX");
    double v   = *state->at("linVelY");
    double w   = *state->at("linVelZ");
    double a   = *state->at("speedSound");
    double rho = *state->at("density");

    // Perform table lookups
    double velT = sqrt(pow(u, 2.0) + pow(v, 2.0) + pow(w, 2.0));

    mach   = velT/a;
    alphaT = acos(u/v);
    
    if (*state->at("isBurnout"))
    {
        dragCoeff = interp2d_eval(cdPowerOffSpline, mach, alphaT, machAcc, alphaAcc);
    }
    else
    {
        dragCoeff = interp2d_eval(cdPowerOnSpline, mach, alphaT, machAcc, alphaAcc);
    }
    
    // Calculate aerodynamic quantities
    dynamicPressure = 0.5*rho*(velT, 2.0);
    dragForce       = dynamicPressure*dragCoeff*refArea;

}

//---------------------------------------------------------------------------//

Aerodynamics::~Aerodynamics()
{

    if (isInit)
    {

        gsl_spline2d_free(cdPowerOffSpline);
        gsl_spline2d_free(cdPowerOnSpline);
        gsl_spline2d_free(clPowerOffSpline);
        gsl_spline2d_free(clPowerOnSpline);
        gsl_spline2d_free(cpSpline);

        gsl_interp_accel_free(machAcc);
        gsl_interp_accel_free(alphaAcc);

    }

}