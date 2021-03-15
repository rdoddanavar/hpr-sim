// System libraries
#include <cstdio>

// External libraries
#include "eigen/Eigen/Core"
#include "eigen/Eigen/Geometry"
#include "eigen/Eigen/LU"

// Project headers
#include "model.h"

//---------------------------------------------------------------------------//

void EOM::initialize()
{

    // Initialize vectors
    force  = Eigen::Vector3d::Zero();
    moment = Eigen::Vector3d::Zero();
    
    linAcc = Eigen::Vector3d::Zero();
    linVel = Eigen::Vector3d::Zero();
    linPos = Eigen::Vector3d::Zero();

    angAcc = Eigen::Vector3d::Zero();
    linVel = Eigen::Vector3d::Zero();
    linPos = Eigen::Vector3d::Zero();

    // Set corresponding state values
    stateInit["forceX"]  = 0.0; stateInit["forceY"]  = 0.0; stateInit["forceZ"]  = 0.0;
    stateInit["momentX"] = 0.0; stateInit["momentY"] = 0.0; stateInit["momentZ"] = 0.0;

    stateInit["linAccX"] = 0.0; stateInit["linAccY"] = 0.0; stateInit["linAccZ"] = 0.0;
    stateInit["linVelX"] = 0.0; stateInit["linVelY"] = 0.0; stateInit["linVelZ"] = 0.0;
    stateInit["linPosX"] = 0.0; stateInit["linPosY"] = 0.0; stateInit["linPosZ"] = 0.0;

    stateInit["angAccX"] = 0.0; stateInit["angAccY"] = 0.0; stateInit["angAccZ"] = 0.0;
    stateInit["angVelX"] = 0.0; stateInit["angVelY"] = 0.0; stateInit["angVelZ"] = 0.0;
    stateInit["angPosX"] = 0.0; stateInit["angPosY"] = 0.0; stateInit["angPosZ"] = 0.0;

    reset(); // Set state to IC's

}

//---------------------------------------------------------------------------//

void EOM::test()
{

    tState["linPosZ"] = state["linPosZ"];
    tState["mass"] = 0.0;
    tState["gravity"] = 0.0;
    tState["thrust"] = 0.0;

    gState = &tState;

    set_gState(gState);

}

//---------------------------------------------------------------------------//

void EOM::update(double timeEval)
{

    state["time"] = timeEval;

    update_deps();

}

//---------------------------------------------------------------------------//

void EOM::update_gState()
{
    ;
}