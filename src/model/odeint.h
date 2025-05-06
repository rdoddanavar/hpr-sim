#pragma once

// System headers
#include <map>
#include <string>

// External headers
#include "eigen/Eigen/Core"

// Internal headers
// <none>

//---------------------------------------------------------------------------//

template<size_t N>
class OdeInt
{

    public:

        void update();

    private:

        Eigen::Matrix<double, N, 1> x;

};
