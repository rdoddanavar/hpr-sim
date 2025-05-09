// System headers
// <none>

// External headers
// <none>

// Internal headers
#include "model.h"

//---------------------------------------------------------------------------//

void Mass::init(double massBodyInit) 
{

    massBody = massBodyInit;

    isInit_ = true;

}

//---------------------------------------------------------------------------//

void Mass::set_state_fields()
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
