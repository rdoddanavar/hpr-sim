// System libraries
#include <cstdio>
#include <vector>

// External libraries
#include "eigen/Eigen/Core"
#include "eigen/Eigen/Geometry"
#include "eigen/Eigen/LU"

// Project headers
#include "model.h"

//---------------------------------------------------------------------------//

void EOM::init()
{

    // Initialize vectors
    force  = Eigen::Vector3d::Zero();
    moment = Eigen::Vector3d::Zero();
    
    linAcc = Eigen::Vector3d::Zero();
    linVel = Eigen::Vector3d::Zero();
    linPos = Eigen::Vector3d::Zero();

    angAcc = Eigen::Vector3d::Zero();
    angVel = Eigen::Vector3d::Zero();
    angPos = Eigen::Vector3d::Zero();
    
    isInit = true;

}

//---------------------------------------------------------------------------//

void EOM::set_state()
{
    
    state->emplace("forceX", &force[0]);
    state->emplace("forceY", &force[1]);
    state->emplace("forceZ", &force[2]);

    state->emplace("linAccX", &linAcc[0]);
    state->emplace("linAccY", &linAcc[1]);
    state->emplace("linAccZ", &linAcc[2]);

    state->emplace("linPosX", &linPos[0]);
    state->emplace("linPosY", &linPos[1]);
    state->emplace("linPosZ", &linPos[2]);

}

//---------------------------------------------------------------------------//

void EOM::init_test()
{
    init_state(&tState);

    state->emplace("time", &time);
    state->emplace("massBody", &massBody);

}

void EOM::test(double timeEval)
{
    
    time = timeEval;
    massBody = 2.0;
    update();

}

//---------------------------------------------------------------------------//

void EOM::update()
{

    update_deps();

    // Populate vectors
    double thrust   = *state->at("thrust");
    double gravity  = *state->at("gravity");
    double massEng  = *state->at("massEng");
    double massBody = *state->at("massBody");

    force[2] = thrust - gravity*(massBody + massEng);

    // Linear EOM
    linAcc = force / (massBody + massEng);

}