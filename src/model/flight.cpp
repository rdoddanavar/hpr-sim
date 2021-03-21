// System libraries
// <none>

// External libraries
#include "gsl/interpolation/gsl_interp.h"
#include "gsl/interpolation/gsl_spline.h"

// Project headers
#include "model.h"

//---------------------------------------------------------------------------//

void Flight::init()
{

    time     = 0.0;
    massBody = 2.0;
    
    state = new stateMap;

    isInit = true;

}

//---------------------------------------------------------------------------//

void Flight::set_state()
{
    state->emplace("time", &time);
    state->emplace("massBody", &massBody);
}

//---------------------------------------------------------------------------//

void Flight::update()
{
    ;
}

//---------------------------------------------------------------------------//

void Flight::update_ode()
{
    ;
}

//---------------------------------------------------------------------------//

Flight::~Flight()
{
    delete state;
}