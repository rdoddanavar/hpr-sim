// System libraries
// <none>

// External libraries
#include "gsl/interpolation/gsl_interp.h"

// Project headers
#include "model.h"
#include "util_model.h"

//---------------------------------------------------------------------------//

// Model Constants

// US Standard Atmosphere 1976 (Table 4.)
const int nlapseRateBin = 7;
const double lapseRateInd[7] = { 0.0e+3, 11.0e+3, 20.0e+3, 32.0e+3, 47.0e+3, 51.0e+3, 71.0e+3}; // [m]
const double lapseRateDep[7] = {-6.5e-3,  0.0e-3,  1.0e-3,  2.8e-3,  0.0e-3, -2.8e-3, -2.0e-3}; // [K/m]

// TODO: add the last altitude value to round out 7 bins

//---------------------------------------------------------------------------//

void Atmosphere::init(double tempInit, double pressInit, double humInit)
{

    temperature = tempInit;
    pressure    = pressInit;
    humidity    = humInit;

    usStd1976_init_temp();

    isInit = true;

}

//---------------------------------------------------------------------------//

void Atmosphere::set_state()
{

    state->emplace("temperature", &temperature);
    //state->emplace("speedSound", &speedSound);
    //state->emplace("dynamicViscosity", &dynamicViscosity);
    //state->emplace("humidity", &humidity);
    //state->emplace("pressure", &pressure);
    //state->emplace("density", &density);

}

//---------------------------------------------------------------------------//

void Atmosphere::update()
{

    update_deps();

    double altitude = *state->at("altitude");
    usStd1976(altitude);

}

//---------------------------------------------------------------------------//

void Atmosphere::usStd1976_init_temp()
{

    // Pre-compute temperature profile
    double altitude = 0.0; // TODO: *state->at("altitude");

    tempProfileInd[0] = altitude; // TODO: better handling for altitude initialization; see Geodetic class
    tempProfileDep[0] = temperature;

    // TODO: what if initial altitude is above first temp bin? highly unlikely but possible
    double tempNext; // [K]

    for (int iBin = 1; iBin < nlapseRateBin; iBin++)
    {
        
        tempProfileInd[iBin] = lapseRateInd[iBin];

        tempNext = tempProfileDep[iBin-1] + lapseRateDep[iBin-1] * (tempProfileInd[iBin] - tempProfileInd[iBin-1]);

        tempProfileDep[iBin] = tempNext;

    }

    // Create interpolant
    interp1d_init(tempInterp, tempProfileInd, tempProfileDep, nlapseRateBin, tempAcc);

}

//---------------------------------------------------------------------------//

void Atmosphere::usStd1976(double altitude)
{

    // US Standard Atmosphere 1976
    temperature = interp1d_eval(tempInterp, tempProfileInd, tempProfileDep, altitude, tempAcc);

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