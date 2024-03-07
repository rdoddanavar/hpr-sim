// System libraries
#include <vector>
#include <iostream>

// External libraries
#include "eigen/Eigen/Core"
#include "eigen/Eigen/Geometry"
#include "eigen/Eigen/LU"

// Project headers
#include "model.h"

//---------------------------------------------------------------------------//

void EOM::init(double launchAzInit, double launchElInit)
{

    // Initialize vectors

    forceB  = Eigen::Vector3d::Zero();
    linAccB = Eigen::Vector3d::Zero();
    linVelB = Eigen::Vector3d::Zero();
    linVelE = Eigen::Vector3d::Zero();
    linPosE = Eigen::Vector3d::Zero();

    momentB = Eigen::Vector3d::Zero();
    angAccB = Eigen::Vector3d::Zero();
    angVelB = Eigen::Vector3d::Zero();
    angVelE = Eigen::Vector3d::Zero();
    angPosE = Eigen::Vector3d::Zero();

    // Find initial position
    launchAz = launchAzInit;
    launchEl = launchElInit;

    double cgX = 0.0;//*state->at("cgX");

    linPosE[0] = cgX*sin(launchAz);
    linPosE[1] = cgX*cos(launchAz);
    linPosE[2] = cgX*sin(launchEl);
    
    isInit = true;

}

//---------------------------------------------------------------------------//

void EOM::set_state()
{
    
    // Linear dynamics

    state->emplace("forceXB", &forceB[0]);
    state->emplace("forceYB", &forceB[1]);
    state->emplace("forceZB", &forceB[2]);

    state->emplace("uDot", &linAccB[0]);
    state->emplace("vDot", &linAccB[1]);
    state->emplace("wDot", &linAccB[2]);

    state->emplace("u", &linVelB[0]);
    state->emplace("v", &linVelB[1]);
    state->emplace("w", &linVelB[2]);

    state->emplace("xDot", &linVelE[0]);   
    state->emplace("yDot", &linVelE[1]);
    state->emplace("zDot", &linVelE[2]);

    state->emplace("x", &linPosE[0]);   
    state->emplace("y", &linPosE[1]);
    state->emplace("z", &linPosE[2]);

    // Angular dynamics

    state->emplace("momentXB", &momentB[0]);
    state->emplace("momentYB", &momentB[1]);
    state->emplace("momentZB", &momentB[2]);

    state->emplace("pDot", &angAccB[0]);
    state->emplace("qDot", &angAccB[1]);
    state->emplace("rDot", &angAccB[2]);

    state->emplace("p", &angVelB[0]);
    state->emplace("q", &angVelB[1]);
    state->emplace("r", &angVelB[2]);

    state->emplace("phiDot",   &angVelE[0]);   
    state->emplace("thetaDot", &angVelE[1]);
    state->emplace("psiDot",   &angVelE[2]);

    state->emplace("phi",   &angPosE[0]);   
    state->emplace("theta", &angPosE[1]);
    state->emplace("psi",   &angPosE[2]);

}

//---------------------------------------------------------------------------//

void EOM::update()
{

    update_deps();

    // Populate vectors
    double thrust  = *state->at("thrust");
    double mass    = *state->at("mass");
    double gravity = *state->at("gravity");

    double forceGrav  = mass*gravity;
    double flightPath;

    if (linVelE[0] == 0.0)
    {
        flightPath = M_PI;
    }
    else if (linVelE[2] == 0.0)
    {
        flightPath = 0.0;
    }
    else
    {
        flightPath = atan2(linVelE[2], linVelE[0]);
    }

    forceB[0] = thrust - forceGrav*sin(flightPath);
    forceB[1] = 0;
    forceB[2] = forceGrav*cos(flightPath);

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
    linVelE[0] = linVelB[0]*cos(flightPath);
    linVelE[2] = linVelB[2]*sin(flightPath);

}
