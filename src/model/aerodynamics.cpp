// System libraries
// <none>

// External libraries
// <none>

// Project headers
#include "model.h"

//---------------------------------------------------------------------------//

void Aerodynamics::init()
{

    dynamicPressure = 0.0;
    mach            = 0.0;
    reynolds        = 0.0;

    isInit = true;

}

//---------------------------------------------------------------------------//

void Aerodynamics::set_state()
{

    state->emplace("dynamicPressure", &dynamicPressure);
    state->emplace("mach", &mach);
    state->emplace("reynolds", &reynolds);

}

//---------------------------------------------------------------------------//

void Aerodynamics::update()
{

    update_deps();

}