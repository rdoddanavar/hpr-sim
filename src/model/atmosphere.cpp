// System libraries
// <none>

// External libraries
// <none>

// Project headers
#include "model.h"

//---------------------------------------------------------------------------//

void Atmosphere::init(double tempInit, double pressInit, double humInit)
{

    temperature = tempInit;
    pressure    = pressInit;
    humidity    = humInit;

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

    double gravity = *state->at("gravity");

}