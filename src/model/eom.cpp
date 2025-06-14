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

    euler   = Eigen::Vector3d::Zero(); // phi, theta, psi

    quat    = Eigen::Quaterniond::Identity(); // q0, q1, q2, q3
    quatDot = Eigen::Vector4d::Zero();        // q0Dot, q1Dot, q2Dot, q3Dot

    // Set initial states
    double cgX = 0.0;//*state->at("cgX");

    linPosE(0) = cgX*sin(launchAz); // East
    linPosE(1) = cgX*cos(launchAz); // North
    linPosE(2) = cgX*sin(launchEl); // Up

    // TODO: check alignment with ENU frame
    euler(0) = 0.0;               // Roll
    euler(1) = launchEl;         // Pitch
    euler(2) = M_PI_2 - launchAz; // Yaw

    quat = Eigen::AngleAxisd(euler(2), Eigen::Vector3d::UnitZ())
         * Eigen::AngleAxisd(euler(1), Eigen::Vector3d::UnitY())
         * Eigen::AngleAxisd(euler(0), Eigen::Vector3d::UnitX());

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
    double thrust  = *state->at("thrust");
    double mass    = *state->at("mass");
    double gravity = *state->at("gravity");

    // Get forces and moments
    double forceGrav = mass*gravity;

    Eigen::Vector3d fThrustB = {thrust, 0.0, 0.0};

    Eigen::Vector3d fGravE = {0.0, 0.0, -mass*gravity};
    quat.normalize();
    Eigen::Vector3d fGravB = quat * fGravE;
    // Rotate fGrav to body frame
    forceB = fThrustB + fGravB;

    // Ground contact condition at launch

    if ((forceB(0) < 0.0) && (!launchFlag))
    {
        forceB = Eigen::Vector3d::Zero();
    }
    else if (!launchFlag)
    {
        launchFlag = true;
    }

    // TODO: Rail force/moment constraint

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
