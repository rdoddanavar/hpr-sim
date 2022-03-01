// System libraries
// <none>

// External libraries
#include "gsl/interpolation/gsl_interp2d.h"
#include "gsl/interpolation/gsl_spline2d.h"

// Project headers
#include "model.h"
#include "util_model.h"

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
    state->emplace("angleAttack", &angleAttack);
    state->emplace("dragCoeff", &dragCoeff);
    state->emplace("liftCoeff", &liftCoeff);
    state->emplace("centerPressure", &centerPressure);

}

//---------------------------------------------------------------------------//

void Aerodynamics::update()
{

    update_deps();

}

//---------------------------------------------------------------------------//

Aerodynamics::~Aerodynamics()
{

    if (isInit)
    {

        gsl_spline2d_free(cdPowerOffSpline);
        gsl_spline2d_free(cdPowerOnSpline);
        gsl_spline2d_free(clPowerOffSpline);
        gsl_spline2d_free(clPowerOnSpline);
        gsl_spline2d_free(cpSpline);

        gsl_interp_accel_free(cdPowerOffAcc);
        gsl_interp_accel_free(cdPowerOnAcc);
        gsl_interp_accel_free(clPowerOffAcc);
        gsl_interp_accel_free(clPowerOnAcc);
        gsl_interp_accel_free(cpAcc);

    }

}