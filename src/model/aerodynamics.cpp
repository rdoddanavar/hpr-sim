// System libraries
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>

// External libraries
#include "pybind11/numpy.h"
#include "gsl/interpolation/gsl_interp2d.h"
#include "gsl/interpolation/gsl_spline2d.h"

// Project headers
#include "model.h"

//---------------------------------------------------------------------------//

void Aerodynamics::init(const double&      refArea  ,
                        const numpyArray& machArray      ,
                        const numpyArray& alphaArray,
                        const numpyArray& cpTotalArray   ,
                        const numpyArray& clPowerOffArray,
                        const numpyArray& cdPowerOffArray,
                        const numpyArray& clPowerOnArray ,
                        const numpyArray& cdPowerOnArray )
{

    // Get independent data
    std::vector<double> machData = process_numpy_array(machArray);
    std::vector<double> alphaData = process_numpy_array(alphaArray);

    // Format independent data arrays into unique, monotonically increasing sequences
    std::sort(machData.begin(), machData.end());
    std::sort(alphaData.begin(), alphaData.end());

    machData.erase(std::unique(machData.begin(), machData.end()), machData.end());
    alphaData.erase(std::unique(alphaData.begin(), alphaData.end()), alphaData.end());

    std::vector<std::vector<double>> indData = {machData, alphaData};

    // Get dependent data
    std::vector<double> cpTotalData    = process_numpy_array(cpTotalArray);
    std::vector<double> clPowerOffData = process_numpy_array(clPowerOffArray);
    std::vector<double> cdPowerOffData = process_numpy_array(cdPowerOffArray);
    std::vector<double> clPowerOnData  = process_numpy_array(clPowerOnArray);
    std::vector<double> cdPowerOnData  = process_numpy_array(cdPowerOnArray);

    // Setup interpolation objects
    cpTotalInterp_   .init(indData, cpTotalData   , BILINEAR);
    clPowerOffInterp_.init(indData, clPowerOffData, BILINEAR);
    cdPowerOffInterp_.init(indData, cdPowerOffData, BILINEAR);
    clPowerOnInterp_ .init(indData, clPowerOnData , BILINEAR);
    cdPowerOnInterp_ .init(indData, cdPowerOnData , BILINEAR);

    refArea_ = refArea;

    isInit_ = true;

}

//---------------------------------------------------------------------------//

void Aerodynamics::set_state_fields()
{

    state->emplace("dynamicPressure", &dynamicPressure_);
    state->emplace("mach"           , &mach_           );
    state->emplace("reynolds"       , &reynolds_       );
    state->emplace("alphaT"         , &alphaT_         );
    state->emplace("dragCoeff"      , &dragCoeff_      );
    state->emplace("liftCoeff"      , &liftCoeff_      );
    state->emplace("centerPressure" , &centerPressure_ );
    state->emplace("dragForce"      , &dragForce_      );
    state->emplace("liftForce"      , &liftForce_      );

}

//---------------------------------------------------------------------------//

void Aerodynamics::update()
{

    update_deps();

    // Get state data
    double u   = *state->at("linVelZ");
    double v   = *state->at("linVelY");
    double w   = *state->at("linVelX");
    double a   = *state->at("speedSound");
    double rho = *state->at("density");

    // Perform table lookups
    double velT = sqrt(pow(u, 2.0) + pow(v, 2.0) + pow(w, 2.0));

    if (velT > 0.0)
    {
        mach_   = velT/a;
        alphaT_ = acos(abs(u)/velT);
    }
    else
    {
        mach_   = 0.0;
        alphaT_ = 0.0;
    }

    std::vector<double> indData = {mach_, alphaT_};

    if (*state->at("isBurnout"))
    {
        dragCoeff_ = cdPowerOffInterp_.update(indData);
        liftCoeff_ = clPowerOffInterp_.update(indData);
    }
    else
    {
        dragCoeff_ = cdPowerOnInterp_.update(indData);
        liftCoeff_ = clPowerOnInterp_.update(indData);
    }

    // Calculate aerodynamic quantities
    dynamicPressure_ = 0.5*rho*(velT*velT);
    dragForce_       = dynamicPressure_*dragCoeff_*refArea_;
    liftForce_       = dynamicPressure_*liftCoeff_*refArea_;

}
