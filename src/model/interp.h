#pragma once

#include <cstddef>
#include <vector>
#include "pybind11/numpy.h"

#include "gsl/interpolation/gsl_interp.h"
#include "gsl/interpolation/gsl_interp2d.h"
#include "gsl/interpolation/gsl_spline.h"
#include "gsl/interpolation/gsl_spline2d.h"

namespace py = pybind11;
using numpyArray = py::array_t<double, py::array::c_style | py::array::forcecast>;

//---------------------------------------------------------------------------//

std::vector<double> process_numpy_array(const numpyArray& array);

//---------------------------------------------------------------------------//

enum interpMethod
{
    NONE     = 0, // TODO: NEAREST method?
    LINEAR   = 1,
    PCHIP    = 2,
    BILINEAR = 3
};

//---------------------------------------------------------------------------//

class Interp
{

    public:

        void init(std::vector<std::vector<double>> dataInd, std::vector<double> dataDep, interpMethod method);
        double update(double xq);
        double update(std::vector<double> xq);

    private:

        void init_linear();
        double update_linear(double xq);
        void search(size_t iDim, double xq);

        std::vector<std::vector<double>> dataInd_;
        std::vector<double> dataDep_;

        interpMethod method_;

        size_t nDim_;

        std::vector<double> xMin_;
        std::vector<double> xMax_;

        std::vector<size_t> iSearch_;

};
