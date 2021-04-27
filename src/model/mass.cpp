// System libraries
// <none>

// External libraries
// <none>

// Project headers
#include "model.h"

//---------------------------------------------------------------------------//

void Mass::init(double massBodyInit) 
{

    massBody = massBodyInit;

    isInit = true;

}

//---------------------------------------------------------------------------//

void Mass::set_state()
{
    state->emplace("mass" , &mass);
}

//---------------------------------------------------------------------------//

void Mass::update()
{

    update_deps();

    double massEng = *state->at("massEng");

    mass = massBody + massEng;

}