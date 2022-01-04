// System libraries
#include <cmath>

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

const double gamma       = 1.4;
const double gasConstAir = 287.05; // TODO: replace this with a more precise number: Rsp = R/M

// TODO: add the last altitude value to round out 7 bins

//---------------------------------------------------------------------------//

void Atmosphere::init(double tempInit, double pressInit)
{

    temperature = tempInit;
    pressure    = pressInit;

    usStd1976_init_temp();

    isInit = true;

}

//---------------------------------------------------------------------------//

void Atmosphere::set_state()
{

    state->emplace("temperature", &temperature);
    //state->emplace("speedSound", &speedSound);
    //state->emplace("dynamicViscosity", &dynamicViscosity);
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

    tempProfileInd.push_back(altitude); // TODO: better handling for altitude initialization; see Geodetic class
    tempProfileDep.push_back(temperature);

    // TODO: what if initial altitude is above first temp bin? highly unlikely but possible
    double tempNext; // [K]

    for (int iBin = 1; iBin < nlapseRateBin; iBin++)
    {
        
        tempProfileInd.push_back(lapseRateInd[iBin]);

        tempNext = tempProfileDep[iBin-1] + lapseRateDep[iBin-1] * (tempProfileInd[iBin] - tempProfileInd[iBin-1]);

        tempProfileDep.push_back(tempNext);

    }

    // Create interpolant
    interp1d_init(tempInterp, tempProfileInd.data(), tempProfileDep.data(), tempProfileInd.size(), tempAcc);

}

//---------------------------------------------------------------------------//

void Atmosphere::usStd1976(double altitude)
{

    // US Standard Atmosphere 1976
    temperature = interp1d_eval(tempInterp, tempProfileInd.data(), tempProfileDep.data(), altitude, tempAcc);

    speedSound = sqrt(gamma * gasConstAir * temperature); 

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