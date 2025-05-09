#pragma once

// System headers
#include <map>
#include <string>

// External headers
#include "eigen/Eigen/Core"

// Internal headers


//---------------------------------------------------------------------------//

typedef void (*OdeFun)(double t, double* y, double* f, void* params);

template<size_t N>
class OdeInt
{

    public:

        void init(double dt, OdeFun odeFun, void* params);
        double* update(double t);

    private:

        Eigen::Matrix<double, N, 1> y_;
        Eigen::Matrix<double, N, 1> f_;

        OdeFun odeFun_ {nullptr};

        void* params_;

        double dt_;

        Eigen::Matrix<double, N, 1> k1;
        Eigen::Matrix<double, N, 1> k2;
        Eigen::Matrix<double, N, 1> k3;
        Eigen::Matrix<double, N, 1> k4;

        bool isInit_ {false};

};

template<size_t N>

void OdeInt<N>::init(double dt, OdeFun odeFun, void* params)
{
    dt_     = dt;
    odeFun_ = odeFun;
    params_ = params;
}

template<size_t N>
double* OdeInt<N>::update(double t)
{

    Eigen::Matrix<double, N, 1> y;

    y = y_;
    odeFun_(t, y.data(), f_.data(), params_);
    k1 = f_;

    y = y_ + k1*dt_/2;
    odeFun_(t + dt_/2, y.data(), f_.data(), params_);
    k2 = f_;

    y = y_ + k2*dt_/2;
    odeFun_(t + dt_/2, y.data(), f_.data(), params_);
    k3 = f_;

    y = y_ + k3*dt_;
    odeFun_(t + dt_, y.data(), f_.data(), params_);
    k4 = f_;

    y_ += dt_/6*(k1 + 2*k2 + 2*k3 + k4);

    return y_.data();

}
