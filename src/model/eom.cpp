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

    force  = Eigen::Vector3d::Zero();
    moment = Eigen::Vector3d::Zero();
    
    linAcc = Eigen::Vector3d::Zero();
    angAcc = Eigen::Vector3d::Zero();

}

//---------------------------------------------------------------------------//

void EOM::update(double val)
{

    printf("value: %f\n", force[0]);

}