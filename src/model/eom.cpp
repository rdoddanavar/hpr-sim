// System headers
#include <vector>

// External headers
#include "eigen/Eigen/Core"
#include "eigen/Eigen/Geometry"
#include "eigen/Eigen/LU"

// Internal headers
#include "model.h"

//---------------------------------------------------------------------------//

void EOM::init(double launchAz, double launchEl)
{

    // Initialize vectors
    // B: "body"
    // E: "earth" (flat earth, non-rotating, ENU frame)

    forceB  = Eigen::Vector3d::Zero(); // fx, fy, fz
    linAccB = Eigen::Vector3d::Zero(); // uDot, vDot, wDot
    linVelB = Eigen::Vector3d::Zero(); // u, v, w
    linVelE = Eigen::Vector3d::Zero(); // xDot, yDot, zDot
    linPosE = Eigen::Vector3d::Zero(); // x, y, z

    momentB = Eigen::Vector3d::Zero(); // mx, my, mz
    angAccB = Eigen::Vector3d::Zero(); // wxDot, wyDot, wzDot
    angVelB = Eigen::Vector3d::Zero(); // wx, wy, wz
    angPosE = Eigen::Vector3d::Zero(); // phi, theta, psi

    q    = Eigen::Quaterniond::Identity(); // qw, qx, qy, qz
    qDot = Eigen::Vector4d::Zero(); // qwDot, qxDot, qyDot, qzDot

    // Set initial states
    double cgX = 0.0;//*state->at("cgX");

    linPosE[0] = cgX*sin(launchAz); // East
    linPosE[1] = cgX*cos(launchAz); // North
    linPosE[2] = cgX*sin(launchEl); // Up

    angPosE[0] = 0.0;               // Roll
    angPosE[1] = launchAz;          // Pitch
    angPosE[2] = M_PI/2 - launchAz; // Yaw

    // TODO: verify Eigen operations
    q = Eigen::AngleAxisd(angPosE[0], Eigen::Vector3d::UnitX())
      * Eigen::AngleAxisd(angPosE[1], Eigen::Vector3d::UnitY())
      * Eigen::AngleAxisd(angPosE[2], Eigen::Vector3d::UnitZ());

    isInit_ = true;

}

//---------------------------------------------------------------------------//

void EOM::set_state_fields()
{

    // Linear dynamics

    state->emplace("forceXB", &forceB[0] );
    state->emplace("forceYB", &forceB[1] );
    state->emplace("forceZB", &forceB[2] );

    state->emplace("uDot"   , &linAccB[0]);
    state->emplace("vDot"   , &linAccB[1]);
    state->emplace("wDot"   , &linAccB[2]);

    state->emplace("u"      , &linVelB[0]);
    state->emplace("v"      , &linVelB[1]);
    state->emplace("w"      , &linVelB[2]);

    state->emplace("xDot"   , &linVelE[0]);
    state->emplace("yDot"   , &linVelE[1]);
    state->emplace("zDot"   , &linVelE[2]);

    state->emplace("x"      , &linPosE[0]);
    state->emplace("y"      , &linPosE[1]);
    state->emplace("z"      , &linPosE[2]);

    // Angular dynamics

    state->emplace("momentXB", &momentB[0]);
    state->emplace("momentYB", &momentB[1]);
    state->emplace("momentZB", &momentB[2]);

    state->emplace("pDot"    , &angAccB[0]);
    state->emplace("qDot"    , &angAccB[1]);
    state->emplace("rDot"    , &angAccB[2]);

    state->emplace("p"       , &angVelB[0]);
    state->emplace("q"       , &angVelB[1]);
    state->emplace("r"       , &angVelB[2]);

    state->emplace("phi"     , &angPosE[0]);
    state->emplace("theta"   , &angPosE[1]);
    state->emplace("psi"     , &angPosE[2]);

    state->emplace("qwDot"   , &qDot[0]   );
    state->emplace("qxDot"   , &qDot[1]   );
    state->emplace("qyDot"   , &qDot[2]   );
    state->emplace("qzDot"   , &qDot[3]   );

    state->emplace("qw"      , &q.w()     );
    state->emplace("qx"      , &q.x()     );
    state->emplace("qy"      , &q.y()     );
    state->emplace("qz"      , &q.z()     );

}

//---------------------------------------------------------------------------//

void EOM::update()
{

    update_deps();

    // Populate vectors
    double thrust  = *state->at("thrust");
    double mass    = *state->at("mass");
    double gravity = *state->at("gravity");

    double forceGrav = mass*gravity;

    Eigen::Vector3d fThrust = {thrust, 0.0, 0.0};

    Eigen::Vector3d fGrav = {0.0, 0.0, -mass*gravity};
    // Rotate fGrav to body frame
    forceB = fThrust + fGrav;

    // Ground contact condition at launch

    if ((forceB[0] < 0.0) && (!launchFlag))
    {
        forceB = Eigen::Vector3d::Zero();
    }
    else if (!launchFlag)
    {
        launchFlag = true;
    }

    // Linear EOM
    linAccB = (forceB/mass); //- angVelB.cross(linVelB);

    // Populate states
    // linVelB --> linVelE
    // angVelB --> qDot
    // q       --> angPosE 

}
