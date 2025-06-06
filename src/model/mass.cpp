// System headers
// <none>

// External headers
// <none>

// Internal headers
#include "model.h"

//---------------------------------------------------------------------------//

void Mass::init(double massBody, double inertiaX, double inertiaY, double inertiaZ) 
{

    massBody_ = massBody;
    inertiaX_ = inertiaX;
    inertiaX_ = inertiaY;
    inertiaX_ = inertiaZ;

    isInit_ = true;

}

//---------------------------------------------------------------------------//

void Mass::set_state_fields()
{

    state->emplace("mass" , &mass_);
    state->emplace("inertiaX" , &inertiaX_);
    state->emplace("inertiaY" , &inertiaY_);
    state->emplace("inertiaZ" , &inertiaZ_);

}

//---------------------------------------------------------------------------//

void Mass::update()
{

    update_deps();

    double massEng = *state->at("massEng");

    mass_ = massBody_ + massEng;

}
