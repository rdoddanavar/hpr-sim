// System headers
#include <vector>

// External headers
#include "eigen/Eigen/Core"
#include "eigen/Eigen/Geometry"
#include "eigen/Eigen/LU"

// Internal headers
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

    isInit_ = true;

}

//---------------------------------------------------------------------------//

void EOM::set_state_fields()
{

    state->emplace("forceZ", &force[2]);

    state->emplace("linAccZ", &linAcc[2]);

    state->emplace("linVelX", &linVel[0]);   
    state->emplace("linVelY", &linVel[1]);
    state->emplace("linVelZ", &linVel[2]);

    state->emplace("linPosZ", &linPos[2]);

}

//---------------------------------------------------------------------------//

void EOM::update()
{

    update_deps();

    // Populate vectors
    double thrust  = *state->at("thrust");
    double mass    = *state->at("mass");
    double gravity = *state->at("gravity");

    force[2] = thrust - mass*gravity;

    // Ground contact condition at launch

    if ((force[2] < 0.0) && (!launchFlag))
    {
        force[2] = 0.0;
    }
    else if (!launchFlag)
    {
        launchFlag = true;
    }

    // Linear EOM
    linAcc = force / mass;

}
