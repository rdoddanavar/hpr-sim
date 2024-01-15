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
const std::array<double, 8> tempGradInd = { 0.0e+3, 11.0e+3, 20.0e+3, 32.0e+3, 47.0e+3, 51.0e+3, 71.0e+3, 84.8520e3}; // [m]
const std::array<double, 7> tempGradDep = {-6.5e-3,  0.0e-3,  1.0e-3,  2.8e-3,  0.0e-3, -2.8e-3, -2.0e-3};            // [K/m]
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

    double altitudeGP0 = *state->at("altitudeGP");
    gravity0 = *state->at("gravity0");
    usStd1976_init(altitudeGP0);

    isInit = true;

}

//---------------------------------------------------------------------------//

void Atmosphere::set_state()
{

    state->emplace("temperature", &temperature);
    state->emplace("pressure", &pressure);
    state->emplace("density", &density);
    state->emplace("speedSound", &speedSound);
    state->emplace("dynamicViscosity", &dynamicViscosity);

}

//---------------------------------------------------------------------------//

void Atmosphere::update()
{

    update_deps();

    double altitudeGP = *state->at("altitudeGP");
    usStd1976(altitudeGP);
    sutherland();

}

//---------------------------------------------------------------------------//

void Atmosphere::usStd1976_init(double altitudeGP0)
{

    // Pre-compute temperature & pressure profiles

    profileAlt = std::vector<double>(tempGradInd.begin(), tempGradInd.end());
    profileTemp.resize(profileAlt.size());
    profilePress.resize(profileAlt.size());

    profileAlt[0]   = altitudeGP0;
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

void Atmosphere::usStd1976(double altitudeGP)
{

    // US Standard Atmosphere 1976

    for (int iAlt = 0; iAlt < tempGradDep.size(); iAlt++)
    {

        if ((altitudeGP >= tempGradInd[iAlt]) && (altitudeGP <= tempGradInd[iAlt+1]))
        {

            double dh = altitudeGP - profileAlt[iAlt];

            temperature = profileTemp[iAlt] + tempGradDep[iAlt] * dh;

            if (abs(tempGradDep[iAlt]) < epsilon) // Isothermal region
            {
                pressure = profilePress[iAlt] * exp(-(gravity0 / (gasConstAir * profileTemp[iAlt])) * dh);
            }
            else // Gradient region
            {
                double tempRatio = temperature / profileTemp[iAlt];
                pressure = profilePress[iAlt] * pow(tempRatio, (-gravity0 / (tempGradDep[iAlt] * gasConstAir)));
            }

            break;

        }

    }

    density     = pressure / (gasConstAir * temperature);
    speedSound  = sqrt(gammaAir * gasConstAir * temperature);

}

//---------------------------------------------------------------------------//

void Atmosphere::sutherland()
{
    dynamicViscosity = viscRef * pow((temperature/tempRef), 1.5) * (tempRef + tempSuth)/(temperature + tempSuth);
}
