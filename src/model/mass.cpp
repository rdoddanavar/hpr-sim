// System headers
// <none>

// External headers
// <none>

// Internal headers
#include "model.h"

//---------------------------------------------------------------------------//

void Mass::init(double massBody, double centerGravX, double inertiaX, double inertiaY, double inertiaZ) 
{

    massBody_ = massBody;

    centerGravX_ = centerGravX;
    centerGravY_ = 0.0;
    centerGravZ_ = 0.0;

    inertiaX_ = inertiaX;
    inertiaY_ = inertiaY;
    inertiaZ_ = inertiaZ;

    // TODO: mass and inertias cannot be zero, validate these inputs
    // Consider adding <, <=, >, >= conditions in input file preprocessing

    isInit_ = true;

}

//---------------------------------------------------------------------------//

void Mass::set_state_fields()
{

    state->emplace("mass"       , &mass_       );
    state->emplace("centerGravX", &centerGravX_);
    state->emplace("centerGravY", &centerGravY_);
    state->emplace("centerGravZ", &centerGravZ_);
    state->emplace("inertiaX"   , &inertiaX_   );
    state->emplace("inertiaY"   , &inertiaY_   );
    state->emplace("inertiaZ"   , &inertiaZ_   );

}

//---------------------------------------------------------------------------//

void Mass::update()
{

    update_deps();

    double massEng = *state->at("massEng");

    mass_ = massBody_ + massEng;

}
