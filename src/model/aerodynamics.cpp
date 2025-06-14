// System headers
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>
#include <cmath>

// External headers
#include "pybind11/numpy.h"
#include "eigen/Eigen/Core"
#include "eigen/Eigen/Geometry"
#include "eigen/Eigen/LU"

// Internal headers
#include "model.h"

//---------------------------------------------------------------------------//

void Aerodynamics::init(const double&      refArea       ,
                        const numpyArray& machArray      ,
                        const numpyArray& alphaArray     ,
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
    cpTotalInterp_   .init(indData, cpTotalData   , Interp::BILINEAR);
    clPowerOffInterp_.init(indData, clPowerOffData, Interp::BILINEAR);
    cdPowerOffInterp_.init(indData, cdPowerOffData, Interp::BILINEAR);
    clPowerOnInterp_ .init(indData, clPowerOnData , Interp::BILINEAR);
    cdPowerOnInterp_ .init(indData, cdPowerOnData , Interp::BILINEAR);

    refArea_ = refArea;
    refDia_  = 2.0*sqrt(refArea_/M_PI);

    isInit_ = true;

}

//---------------------------------------------------------------------------//

void Aerodynamics::set_state_fields()
{

    state->emplace("dynamicPressure", &dynamicPressure_);
    state->emplace("mach"           , &mach_           );
    state->emplace("reynolds"       , &reynolds_       );
    state->emplace("alphaT"         , &alphaT_         );
    state->emplace("phiA"           , &phiA_           );
    state->emplace("dragCoeff"      , &dragCoeff_      );
    state->emplace("liftCoeff"      , &liftCoeff_      );
    state->emplace("centerPressure" , &centerPressure_ );
    state->emplace("staticMargin"   , &staticMargin_   );
    state->emplace("dragForce"      , &dragForce_      );
    state->emplace("liftForce"      , &liftForce_      );
    state->emplace("axialForce"     , &axialForce_     );
    state->emplace("normalForce"    , &normalForce_    );
    state->emplace("aeroForceX"     , &aeroForce_(0)   );
    state->emplace("aeroForceY"     , &aeroForce_(1)   );
    state->emplace("aeroForceZ"     , &aeroForce_(2)   );
    state->emplace("aeroMomentX"    , &aeroMoment_(0)  );
    state->emplace("aeroMomentY"    , &aeroMoment_(1)  );
    state->emplace("aeroMomentZ"    , &aeroMoment_(2)  );

}

//---------------------------------------------------------------------------//

void Aerodynamics::update()
{

    update_deps();

    // Get state data
    double u   = *state->at("linVelXB");
    double v   = *state->at("linVelYB");
    double w   = *state->at("linVelZB");
    double a   = *state->at("speedSound");
    double rho = *state->at("density");
    double mu  = *state->at("dynamicViscosity");

    // double u = *state->at("linVelXB") - *state->at("windXB");
    // double v = *state->at("linVelYB") - *state->at("windYB");
    // double w = *state->at("linVelZB") - *state->at("windZB");

    double velT = sqrt(pow(u, 2) + pow(v, 2) + pow(w, 2));

    dynamicPressure_ = 0.5*rho*pow(velT, 2);
    reynolds_ = rho*velT*refDia_/mu; // TODO: check ref length

    // Perform table lookups
    mach_   = velT/a;
    alphaT_ = acos(abs(u)/velT);
    phiA_   = atan2(v, w);

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

    // Dimensionalize forces
    dragForce_ = dynamicPressure_*dragCoeff_*refArea_;
    liftForce_ = dynamicPressure_*liftCoeff_*refArea_;

    // Perform rotations
    Eigen::AngleAxisd rotAlphaT(alphaT_, Eigen::Vector3d::UnitY());
    Eigen::AngleAxisd rotPhiA  (phiA_  , Eigen::Vector3d::UnitX());

    Eigen::Vector3d forceDYL = {dragForce_ , 0.0, liftForce_};
    Eigen::Vector3d forceAYN = rotAlphaT*forceDYL;

    axialForce_  = forceAYN(0);
    normalForce_ = forceAYN(2);

    Eigen::Vector3d forceXYZ = {-axialForce_, 0.0, -normalForce_};

    // TODO: check this rotation
    aeroForce_ = rotPhiA*forceXYZ; // Body frame

    // Compute aerodynamic moment
    centerPressure_ = cpTotalInterp_.update(indData);

    Eigen::Vector3d cpXYZ = {-centerPressure_, 0.0, 0.0};

    Eigen::Vector3d cgXYZ = {
        *state->at("centerGravX"),
        *state->at("centerGravY"),
        *state->at("centerGravZ")
    };

    Eigen::Vector3d momArm = cgXYZ - cpXYZ;

    staticMargin_ = momArm(0) / refDia_;
    aeroMoment_   = aeroForce_.cross(momArm);

}
