// System libraries
#include <cmath>

// External libraries
#include "gsl/interpolation/gsl_interp.h"

// Project headers
#include "model.h"
#include "util_model.h"

//---------------------------------------------------------------------------//

// Model Constants

// US Standard Atmosphere 1976: Table 4.
const std::array<double, 8> lapseRateInd = { 0.0e+3, 11.0e+3, 20.0e+3, 32.0e+3, 47.0e+3, 51.0e+3, 71.0e+3, 84.8520e3}; // [m]
const std::array<double, 7> lapseRateDep = {-6.5e-3,  0.0e-3,  1.0e-3,  2.8e-3,  0.0e-3, -2.8e-3, -2.0e-3};            // [K/m]

// US Standard Atmosphere 1976: (univ. gas const.) / (molar mass air)
const double gasConstAir = 8.31432e3/28.9644; // [J/(kg*K)]

const double gammaAir = 1.4;

//---------------------------------------------------------------------------//

void Atmosphere::init(double tempInit, double pressInit)
{

    temperature = tempInit;
    pressure    = pressInit;

    double altitudeMSL0 = *state->at("altitudeMSL");
    usStd1976_init(altitudeMSL0);

    isInit = true;

}

//---------------------------------------------------------------------------//

void Atmosphere::set_state()
{

    state->emplace("temperature", &temperature);
    state->emplace("speedSound", &speedSound);
    //state->emplace("dynamicViscosity", &dynamicViscosity);
    //state->emplace("pressure", &pressure);
    //state->emplace("density", &density);

}

//---------------------------------------------------------------------------//

void Atmosphere::update()
{

    update_deps();

    double altitudeMSL = *state->at("altitudeMSL");
    usStd1976(altitudeMSL);

}

//---------------------------------------------------------------------------//

void Atmosphere::usStd1976_init(double altitudeMSL0)
{

    // Pre-compute temperature profile

    tempProfileInd.resize(lapseRateInd.size());
    tempProfileDep.resize(lapseRateInd.size());

    // TODO: conversion between geometric & geopotential altitudes; do this in Geodetic class?

    tempProfileInd[0] = altitudeMSL0;
    tempProfileDep[0] = temperature;

    // TODO: what if initial altitude is above first temp bin? highly unlikely but possible
    double tempNext; // [K]

    for (int iAlt = 1; iAlt < tempProfileInd.size(); iAlt++)
    {
        
        tempProfileInd[iAlt] = lapseRateInd[iAlt];

        tempNext = tempProfileDep[iAlt-1] + lapseRateDep[iAlt-1] * (tempProfileInd[iAlt] - tempProfileInd[iAlt-1]);

        tempProfileDep[iAlt] = tempNext;

    }

    // Create interpolant
    interp1d_init(tempInterp, tempProfileInd.data(), tempProfileDep.data(), tempProfileInd.size(), tempAcc);

}

//---------------------------------------------------------------------------//

void Atmosphere::usStd1976(double altitudeMSL)
{

    // US Standard Atmosphere 1976
    temperature = interp1d_eval(tempInterp, tempProfileInd.data(), tempProfileDep.data(), altitudeMSL, tempAcc);

    speedSound = sqrt(gammaAir * gasConstAir * temperature); 

}

//---------------------------------------------------------------------------//

Atmosphere::~Atmosphere()
{

    if (isInit)
    {
        
        gsl_interp_free(tempInterp);
        gsl_interp_accel_free(tempAcc);

    }

}