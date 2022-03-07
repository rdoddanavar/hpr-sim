// System libraries
#include <iostream>

// External libraries
#include "pybind11/numpy.h"
#include "gsl/interpolation/gsl_interp2d.h"
#include "gsl/interpolation/gsl_spline2d.h"

// Project headers
#include "model.h"
#include "util_model.h"

//---------------------------------------------------------------------------//

// void Aerodynamics::init(py::array_t<double> machInit      , 
//                         py::array_t<double> alphaInit     ,
//                         py::array_t<double> cdPowerOffInit,
//                         py::array_t<double> cdPowerOnInit ,
//                         py::array_t<double> clPowerOffInit,
//                         py::array_t<double> clPowerOnInit ,
//                         double              refAreaInit   )
void Aerodynamics::init(py::array_t<double> test) 
{

// TODO: Runtime checks on input array dimensions, length

    py::buffer_info buff = test.request();

    double* ptr = (double*) buff.ptr;

    std::cout << "Dim 1: " << buff.shape[0] << std::endl;
    std::cout << "Dim 2: " << buff.shape[1] << std::endl;

    for (int i = 0; i < buff.shape[0]; i++)
    {
        for (int j = 0; j < buff.shape[1]; j++)
        {
            std::cout << "test: " << ptr[i*buff.shape[1] + j] << std::endl;
        }
    }

    isInit = true;

}

//---------------------------------------------------------------------------//

void Aerodynamics::set_state()
{

    state->emplace("dynamicPressure", &dynamicPressure);
    state->emplace("mach", &mach);
    state->emplace("reynolds", &reynolds);
    state->emplace("angleAttack", &angleAttack);
    state->emplace("dragCoeff", &dragCoeff);
    state->emplace("liftCoeff", &liftCoeff);
    state->emplace("centerPressure", &centerPressure);

}

//---------------------------------------------------------------------------//

void Aerodynamics::update()
{

    update_deps();

}

//---------------------------------------------------------------------------//

Aerodynamics::~Aerodynamics()
{

    if (isInit)
    {

        gsl_spline2d_free(cdPowerOffSpline);
        gsl_spline2d_free(cdPowerOnSpline);
        gsl_spline2d_free(clPowerOffSpline);
        gsl_spline2d_free(clPowerOnSpline);
        gsl_spline2d_free(cpSpline);

        gsl_interp_accel_free(machAcc);
        gsl_interp_accel_free(alphaAcc);

    }

}