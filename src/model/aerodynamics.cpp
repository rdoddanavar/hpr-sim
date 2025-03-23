// System libraries
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>

// External libraries
#include "pybind11/numpy.h"
#include "gsl/interpolation/gsl_interp2d.h"
#include "gsl/interpolation/gsl_spline2d.h"

// Project headers
#include "model.h"

//---------------------------------------------------------------------------//

void Aerodynamics::init(const double&      refAreaInit  ,
                        const numpyArray& machInit      ,
                        const numpyArray& alphaInit     ,
                        const numpyArray& cpTotalInit   ,
                        const numpyArray& clPowerOffInit,
                        const numpyArray& cdPowerOffInit,
                        const numpyArray& clPowerOnInit ,
                        const numpyArray& cdPowerOnInit )
{

    refArea = refAreaInit;

    py::buffer_info machBuff       = machInit.request();
    py::buffer_info alphaBuff      = alphaInit.request();
    py::buffer_info cpTotalBuff    = cpTotalInit.request();
    py::buffer_info clPowerOffBuff = clPowerOffInit.request();
    py::buffer_info cdPowerOffBuff = cdPowerOffInit.request();
    py::buffer_info clPowerOnBuff  = clPowerOnInit.request();
    py::buffer_info cdPowerOnBuff  = cdPowerOnInit.request();

    if (machBuff.ndim       != 1 || alphaBuff.ndim      != 1 ||
        cpTotalBuff.ndim    != 1 ||
        clPowerOffBuff.ndim != 1 || cdPowerOffBuff.ndim != 1 ||
        clPowerOnBuff.ndim  != 1 || cdPowerOnBuff.ndim  != 1 )
    {
        throw std::runtime_error("Input arrays must be 1-D");
    }

    const size_t nData = machBuff.size;

    if (alphaBuff.size      != nData ||
        cpTotalBuff.size    != nData ||
        clPowerOffBuff.size != nData || cdPowerOffBuff.size != nData ||
        clPowerOnBuff.size  != nData || cdPowerOnBuff.size  != nData )
    {
        throw std::runtime_error("Input arrays must have identical lengths");
    }

    // Extract data arrays
    double* machData       = (double*) machBuff.ptr;
    double* alphaData      = (double*) alphaBuff.ptr;
    double* cpTotalData    = (double*) cpTotalBuff.ptr;
    double* clPowerOffData = (double*) clPowerOffBuff.ptr;
    double* cdPowerOffData = (double*) cdPowerOffBuff.ptr;
    double* clPowerOnData  = (double*) clPowerOnBuff.ptr;
    double* cdPowerOnData  = (double*) cdPowerOnBuff.ptr;

    // Format independent data arrays into unique, monotonically increasing sequences
    std::vector<double> machVec(machData, machData + nData);
    std::vector<double> alphaVec(alphaData, alphaData + nData);

    std::sort(machVec.begin(), machVec.end());
    std::sort(alphaVec.begin(), alphaVec.end());

    machVec.erase(std::unique(machVec.begin(), machVec.end()), machVec.end());
    alphaVec.erase(std::unique(alphaVec.begin(), alphaVec.end()), alphaVec.end());

    const size_t nMach  = machVec.size();
    const size_t nAlpha = alphaVec.size();

    // Dependent data arrays are given in row-major format
    // Convert to column-major format to satisfy interpolation setup

    std::vector<double> cpTotalVec(nData);
    std::vector<double> clPowerOffVec(nData);
    std::vector<double> cdPowerOffVec(nData);
    std::vector<double> clPowerOnVec(nData);
    std::vector<double> cdPowerOnVec(nData);

    size_t ix, iy, izRow, izCol;

    for (ix = 0; ix < nMach; ix++)
    {
        for (iy = 0; iy < nAlpha; iy++)
        {

            izRow = ix*nAlpha + iy; // Row-major index 
            izCol = iy*nMach  + ix; // Column-major index

            cpTotalVec[izCol]    = cpTotalData[izRow];
            clPowerOffVec[izCol] = clPowerOffData[izRow];
            cdPowerOffVec[izCol] = cdPowerOffData[izRow];
            clPowerOnVec[izCol]  = clPowerOnData[izRow];
            cdPowerOnVec[izCol]  = cdPowerOnData[izRow];

        }
    }

    // Setup interpolation
    machAcc  = gsl_interp_accel_alloc();
    alphaAcc = gsl_interp_accel_alloc();

    interp2d_init(cpTotalSpline,    machVec.data(), alphaVec.data(), cpTotalVec.data(),    nMach, nAlpha);
    interp2d_init(clPowerOffSpline, machVec.data(), alphaVec.data(), clPowerOffVec.data(), nMach, nAlpha);
    interp2d_init(cdPowerOffSpline, machVec.data(), alphaVec.data(), cdPowerOffVec.data(), nMach, nAlpha);
    interp2d_init(clPowerOnSpline,  machVec.data(), alphaVec.data(), clPowerOnVec.data(),  nMach, nAlpha);
    interp2d_init(cdPowerOnSpline,  machVec.data(), alphaVec.data(), cdPowerOnVec.data(),  nMach, nAlpha);

    isInit_ = true;

}

//---------------------------------------------------------------------------//

void Aerodynamics::set_state_fields()
{

    state->emplace("dynamicPressure", &dynamicPressure);
    state->emplace("mach"           , &mach           );
    state->emplace("reynolds"       , &reynolds       );
    state->emplace("alphaT"         , &alphaT         );
    state->emplace("dragCoeff"      , &dragCoeff      );
    state->emplace("liftCoeff"      , &liftCoeff      );
    state->emplace("centerPressure" , &centerPressure );
    state->emplace("dragForce"      , &dragForce      );
    state->emplace("liftForce"      , &liftForce      );

}

//---------------------------------------------------------------------------//

void Aerodynamics::update()
{

    update_deps();

    // Get state data
    double u   = *state->at("linVelZ");
    double v   = *state->at("linVelY");
    double w   = *state->at("linVelX");
    double a   = *state->at("speedSound");
    double rho = *state->at("density");

    // Perform table lookups
    double velT = sqrt(pow(u, 2.0) + pow(v, 2.0) + pow(w, 2.0));

    if (velT > 0.0)
    {
        mach   = velT/a;
        alphaT = acos(abs(u)/velT);
    }
    else
    {
        mach   = 0.0;
        alphaT = 0.0;
    }

    if (*state->at("isBurnout"))
    {
        dragCoeff = interp2d_eval(cdPowerOffSpline, mach, alphaT, machAcc, alphaAcc);
        liftCoeff = interp2d_eval(clPowerOffSpline, mach, alphaT, machAcc, alphaAcc);
    }
    else
    {
        dragCoeff = interp2d_eval(cdPowerOnSpline, mach, alphaT, machAcc, alphaAcc);
        liftCoeff = interp2d_eval(clPowerOnSpline, mach, alphaT, machAcc, alphaAcc);
    }

    // Calculate aerodynamic quantities
    dynamicPressure = 0.5*rho*pow(velT, 2.0);
    dragForce       = dynamicPressure*dragCoeff*refArea;
    liftForce       = dynamicPressure*liftCoeff*refArea;

}

//---------------------------------------------------------------------------//

Aerodynamics::~Aerodynamics()
{

    if (isInit_)
    {

        gsl_spline2d_free(cpTotalSpline);
        gsl_spline2d_free(clPowerOffSpline);
        gsl_spline2d_free(cdPowerOffSpline);
        gsl_spline2d_free(clPowerOnSpline);
        gsl_spline2d_free(cdPowerOnSpline);

        gsl_interp_accel_free(machAcc);
        gsl_interp_accel_free(alphaAcc);

    }

}
