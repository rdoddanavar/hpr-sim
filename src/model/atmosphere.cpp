// System libraries
// <none>

// External libraries
// <none>

// Project headers
#include "model.h"

//---------------------------------------------------------------------------//

// Model Constants

// US Standard Atmosphere 1976 (Table 4.)
const int nlapseRateBin = 7;
const std::array<double, 7> lapseRateInd = { 0.0e+3, 11.0e+3, 20.0e+3, 32.0e+3, 47.0e+3, 51.0e+3, 71.0e+3}; // [m]
const std::array<double, 7> lapseRateDep = {-6.5e-3,  0.0e-3,  1.0e-3,  2.8e-3,  0.0e-3, -2.8e-3, -2.0e-3}; // [K/m]

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
    state->emplace("speedSound", &speedSound);
    state->emplace("dynamicViscosity", &dynamicViscosity);
    state->emplace("humidity", &humidity);
    state->emplace("pressure", &pressure);
    state->emplace("density", &density);

}

//---------------------------------------------------------------------------//

void Atmosphere::update()
{

    update_deps();

    usStd1976();

}

//---------------------------------------------------------------------------//

void Atmosphere::usStd1976_init_temp()
{

    // Pre-compute temperature profile
    double altitude = 0.0; //*state->at("altitude");

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

    for (int iBin = 0; iBin < nlapseRateBin; iBin++)
    {
        std::cout << tempProfileInd[iBin] << " , " << tempProfileDep[iBin] << std::endl;
    }

}

//---------------------------------------------------------------------------//

void Atmosphere::usStd1976()
{

    // US Standard Atmosphere 1976

}