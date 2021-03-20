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
    
    isInit = true;

}

//---------------------------------------------------------------------------//

void EOM::test(double timeEval)
{

    tState["time"]     = timeEval;
    tState["bodyMass"] = 5.0;

    init_gState(tState);
    update(tState);

}

//---------------------------------------------------------------------------//

void EOM::update(stateMap& gState)
{

    update_deps(gState);

    // Populate vectors
    linAcc[2] = gState["thrust"] - gState["gravity"];

    // Linear EOM
    linAcc = force / (gState["bodyMass"] - gState["engMass"]);

    // Update local state
    state["linAccX"] = linAcc[0];
    state["linAccY"] = linAcc[1];
    state["linAccZ"] = linAcc[2];

    update_gState(gState);

}

//---------------------------------------------------------------------------//

void EOM::update_gState(stateMap& gState)
{
    
    std::vector<std::string> keys = {"linAccX", "linAccY", "linAccZ",
                                     "linVelX", "linVelY", "linVelZ",
                                     "linPosX", "linPosY", "linPosZ"};

    for (const auto& key : keys)
    {
        gState[key] = state[key];
    }

}