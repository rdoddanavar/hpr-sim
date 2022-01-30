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
const double epsilon = 1e-6;

// US Standard Atmosphere 1976: (univ. gas const.) / (molar mass air)
const double gasConstAir = 8.31432e3/28.9644; // [J/(kg*K)]

const double gammaAir = 1.4;

// Sutherland's Law
const double viscRef  = 1.716e-5; // [kg/(m*s)]
const double tempRef  = 273.15;   // [K]
const double tempSuth = 110.40;   // [K]

//---------------------------------------------------------------------------//

void Atmosphere::init(double tempInit, double pressInit)
{

    temperature = tempInit;
    pressure    = pressInit;

    double altitudeMSL0 = *state->at("altitudeMSL");
    gravity0 = *state->at("gravity0");
    usStd1976_init(altitudeMSL0);

    isInit = true;

}

//---------------------------------------------------------------------------//

void Atmosphere::set_state()
{

    state->emplace("temperature", &temperature);
    state->emplace("speedSound", &speedSound);
    state->emplace("dynamicViscosity", &dynamicViscosity);
    state->emplace("pressure", &pressure);
    state->emplace("density", &density);

}

//---------------------------------------------------------------------------//

void Atmosphere::update()
{

    update_deps();

    double altitudeMSL = *state->at("altitudeMSL");
    usStd1976(altitudeMSL);
    sutherland();

}

//---------------------------------------------------------------------------//

void Atmosphere::usStd1976_init(double altitudeMSL0)
{

    // Pre-compute temperature profile

    profileAlt = std::vector<double>(lapseRateInd.begin(), lapseRateInd.end());
    profileTemp.resize(profileAlt.size());
    profilePress.resize(profileAlt.size());

    // TODO: conversion between geometric & geopotential altitudes; do this in Geodetic class?

    profileAlt[0]   = altitudeMSL0;
    profileTemp[0]  = temperature;
    profilePress[0] = pressure;

    // TODO: what if initial altitude is above first temp bin? highly unlikely but possible

    for (int iAlt = 1; iAlt < profileAlt.size(); iAlt++)
    {
        
        usStd1976(profileAlt[iAlt]);

        profileTemp[iAlt]  = temperature;
        profilePress[iAlt] = pressure;

    }

}

//---------------------------------------------------------------------------//

void Atmosphere::usStd1976(double altitudeMSL)
{

    // US Standard Atmosphere 1976
    
    for (int iAlt = 0; iAlt < lapseRateDep.size(); iAlt++)
    {
        
        if ((altitudeMSL >= lapseRateInd[iAlt]) && (altitudeMSL <= lapseRateInd[iAlt+1]))
        {
            
            double dh = altitudeMSL - profileAlt[iAlt];

            temperature = profileTemp[iAlt] + lapseRateDep[iAlt] * dh;

            if (abs(lapseRateDep[iAlt]) < epsilon) // Isothermal region
            {
                pressure = profilePress[iAlt] * exp(-(gravity0 / (gasConstAir * profileTemp[iAlt])) * dh);
            }
            else // Gradient region
            {
                double tempRatio = temperature / profileTemp[iAlt];
                pressure = profilePress[iAlt] * pow(tempRatio, (-gravity0 / (lapseRateDep[iAlt] * gasConstAir)));
            }

            break;

        }

    }

    speedSound  = sqrt(gammaAir * gasConstAir * temperature);
    density     = pressure / (gasConstAir * temperature);

}

//---------------------------------------------------------------------------//

void Atmosphere::sutherland()
{
    dynamicViscosity = viscRef * pow((temperature/tempRef), 1.5) * (tempRef + tempSuth)/(temperature + tempSuth);
}