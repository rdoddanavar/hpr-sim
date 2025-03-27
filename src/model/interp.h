#pragma once

#include <cstddef>
#include <vector>
#include "pybind11/numpy.h"

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

        void check_method(std::string methodName, size_t methodDim);
        void init_linear();
        //void init_pchip();
        void init_bilinear();
        double update_linear(double xq);
        double update_bilinear(std::vector<double> xyq);
        size_t search(const std::vector<double>& x, double xq, size_t iSearch);

        std::vector<std::vector<double>> dataInd_;
        std::vector<double> dataDep_;

        interpMethod method_;

        size_t nDim_;

        std::vector<size_t> xSize_;

        std::vector<double> xMin_;
        std::vector<double> xMax_;

        std::vector<size_t> iSearch_;

};
