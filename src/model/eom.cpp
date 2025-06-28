// System headers
#include <vector>

// External headers
#include "eigen/Eigen/Core"
#include "eigen/Eigen/Geometry"
#include "eigen/Eigen/LU"

// Internal headers
#include "model.h"

//---------------------------------------------------------------------------//

void EOM::init(double launchAz, double launchEl, double railLength)
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

    euler   = Eigen::Vector3d::Zero(); // phi, theta, psi

    quat    = Eigen::Quaterniond::Identity(); // q0, q1, q2, q3
    quatDot = Eigen::Vector4d::Zero();        // q0Dot, q1Dot, q2Dot, q3Dot

    // Set initial states
    railLength_  = railLength;
    railPosInit_ = *state->at("bodyLength") + *state->at("centerGravX");

    // TODO: check alignment with ENU frame
    // Euler should be ENU --> Body
    euler(0) = M_PI;              // Roll
    euler(1) = -launchEl;         // Pitch
    euler(2) = M_PI_2 - launchAz; // Yaw

    quat = Eigen::AngleAxisd(euler(2), Eigen::Vector3d::UnitZ())
         * Eigen::AngleAxisd(euler(1), Eigen::Vector3d::UnitY())
         * Eigen::AngleAxisd(euler(0), Eigen::Vector3d::UnitX());

    Eigen::Vector3d linPosB = {railPosInit_, 0.0, 0.0};
    linPosE = quat.conjugate() * linPosB;

    flagRailExit_ = false;

    isInit_ = true;

}

//---------------------------------------------------------------------------//

void EOM::set_state_fields()
{

    // Linear dynamics

    state->emplace("forceXB" , &forceB(0) );
    state->emplace("forceYB" , &forceB(1) );
    state->emplace("forceZB" , &forceB(2) );

    state->emplace("linAccXB", &linAccB(0));
    state->emplace("linAccYB", &linAccB(1));
    state->emplace("linAccZB", &linAccB(2));

    state->emplace("linVelXB", &linVelB(0));
    state->emplace("linVelYB", &linVelB(1));
    state->emplace("linVelZB", &linVelB(2));

    state->emplace("linVelXE", &linVelE(0));
    state->emplace("linVelYE", &linVelE(1));
    state->emplace("linVelZE", &linVelE(2));

    state->emplace("linPosXE", &linPosE(0));
    state->emplace("linPosYE", &linPosE(1));
    state->emplace("linPosZE", &linPosE(2));

    // Angular dynamics

    state->emplace("momentXB", &momentB(0));
    state->emplace("momentYB", &momentB(1));
    state->emplace("momentZB", &momentB(2));

    state->emplace("angAccXB", &angAccB(0));
    state->emplace("angAccYB", &angAccB(1));
    state->emplace("angAccZB", &angAccB(2));

    state->emplace("angVelXB", &angVelB(0));
    state->emplace("angVelYB", &angVelB(1));
    state->emplace("angVelZB", &angVelB(2));

    state->emplace("phi"     , &euler(0)  );
    state->emplace("theta"   , &euler(1)  );
    state->emplace("psi"     , &euler(2)  );

    state->emplace("q0Dot"   , &quatDot(0));
    state->emplace("q1Dot"   , &quatDot(1));
    state->emplace("q2Dot"   , &quatDot(2));
    state->emplace("q3Dot"   , &quatDot(3));

    state->emplace("q0"      , &quat.w()  );
    state->emplace("q1"      , &quat.x()  );
    state->emplace("q2"      , &quat.y()  );
    state->emplace("q3"      , &quat.z()  );

}

//---------------------------------------------------------------------------//

void EOM::update()
{

    update_deps();

    // Populate vectors
    double mass        = *state->at("mass");
    double gravity     = *state->at("gravity");
    double thrust      = *state->at("thrust");
    double aeroForceX  = *state->at("aeroForceX");
    double aeroForceY  = *state->at("aeroForceY");
    double aeroForceZ  = *state->at("aeroForceZ");
    double aeroMomentX = *state->at("aeroMomentX");
    double aeroMomentY = *state->at("aeroMomentY");
    double aeroMomentZ = *state->at("aeroMomentZ");

    // Get forces and moments
    Eigen::Vector3d fThrustB = {thrust     , 0.0        , 0.0          };
    Eigen::Vector3d fGravE   = {0.0        , 0.0        , -mass*gravity};
    Eigen::Vector3d fAeroB   = {aeroForceX , aeroForceY , aeroForceZ   };
    Eigen::Vector3d mAeroB   = {aeroMomentX, aeroMomentY, aeroMomentZ  };

    // Rotate gravity to body frame
    quat.normalize();
    Eigen::Vector3d fGravB = quat*fGravE;

    // Get net force
    forceB = fThrustB + fGravB + fAeroB;

    // Get net moment
    momentB = mAeroB;

    // Assess and apply launch rail constraints

    if (!flagRailExit_)
    {

        Eigen::Vector3d linPosB = quat*linPosE;

        if ((linPosB(0) - railPosInit_) >= railLength_)
        {
            // Assumes motion is constrained by rail until entire body exits
            flagRailExit_ = true;
        }
        else
        {
            // Constraint all motion except that along rail in positive direction
            forceB(0)  = (forceB(0) < 0.0)? 0.0 : forceB(0);
            forceB(1)  = 0.0;
            forceB(2)  = 0.0;
            momentB(0) = 0.0;
            momentB(0) = 0.0;
            momentB(0) = 0.0;
        }
    }

    // Linear EOM
    linAccB = forceB / mass - angVelB.cross(linVelB);

    // Get earth frame velocity
    linVelE = quat.conjugate() * linVelB;

    // Rotational EOM
    Eigen::Matrix3d inertia = Eigen::Matrix3d::Zero();
    inertia(0,0) = *state->at("inertiaX");
    inertia(1,1) = *state->at("inertiaY");
    inertia(2,2) = *state->at("inertiaZ");

    angAccB = inertia.inverse() * (momentB - angVelB.cross(inertia * angVelB));

    // Get matrix of body rates
    double wx = angVelB[0];
    double wy = angVelB[1];
    double wz = angVelB[2];

    Eigen::Matrix4d rateMat
    {
        { 0, -wx, -wy, -wz},
        {wx,   0,  wz, -wy},
        {wy, -wz,   0,  wx},
        {wz,  wy, -wx,   0}
    };

    // Compute quaternion derivative for attitude propogation
    Eigen::Vector4d quatVec = {quat.w(), quat.x(), quat.y(), quat.z()};
    quatDot = rateMat*quatVec;

    // Get Euler angles for convenience
    euler = quat.toRotationMatrix().eulerAngles(2, 1, 0);

}
