#pragma once

// System headers
#include <cstddef>
#include <vector>

// External headers
#include "pybind11/numpy.h"

namespace py = pybind11;
using numpyArray = py::array_t<double, py::array::c_style | py::array::forcecast>;

// Internal headers
// <none>

// Could be inline or constexpr?
template <typename T> int sgn(T x)
{
    return (T(0) < x) - (x < T(0));
}

//---------------------------------------------------------------------------//

std::vector<double> process_numpy_array(const numpyArray& array);

//---------------------------------------------------------------------------//

class Interp
{

    public:

        enum Method
        {
            NONE     = 0, // TODO: NEAREST method?
            LINEAR   = 1,
            PCHIP    = 2,
            BILINEAR = 3
        };

        struct Pchip
        {
            std::vector<double> h;   // Inter-element spacing (dx)
            std::vector<double> del; // Inter-element slopes (dy/dx)
            std::vector<double> d;   // Polynominal derivatives (P'(x))
        };

        void init(std::vector<std::vector<double>> xData, std::vector<double> yData, Method method);
        double update(double xq);
        double update(std::vector<double> xq);

    private:

        void init_linear();
        void init_pchip();
        void init_bilinear();

        void check_method(std::string methodName, size_t methodDim);

        double update_linear(double xq);
        double update_pchip(double xq);
        double update_bilinear(std::vector<double> xq);

        size_t get_2d_idx(size_t i0, size_t i1);
        static size_t search(const std::vector<double>& x, double xq, size_t iS);
        static double deriv_three_point(std::vector<double> x, std::vector<double> y, double xi);

        // Independent data arrays
        std::vector<std::vector<double>> xData_;

        // Dependent data array (row-major)
        std::vector<double> yData_;

        // Interpolation method enumeration
        Method method_; 

        // Number of independent table lookup variables
        size_t nDim_;

        // Independent data array sizes
        std::vector<size_t> xSize_;

        // Independent data min values
        std::vector<double> xMin_;

        // Independent data max values
        std::vector<double> xMax_;

        // Independent data search indices
        std::vector<size_t> iS_;

        // PCHIP method parameters
        Pchip pchip_;

};
