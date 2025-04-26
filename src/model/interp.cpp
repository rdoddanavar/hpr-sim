// System headers
#include <algorithm>
#include <array>
#include <cmath>
#include <stdexcept>

// External headers
// <none>

// Internal headers
#include "interp.h"

//---------------------------------------------------------------------------//

std::vector<double> process_numpy_array(const numpyArray& array)
{

    py::buffer_info buff = array.request();

    if (buff.ndim != 1)
    {
        throw std::runtime_error("NumPy array must be 1-D");
    }

    double* ptr = static_cast<double*>(buff.ptr);

    return std::vector<double>(ptr, ptr + buff.size);

}

//---------------------------------------------------------------------------//

void Interp::init(std::vector<std::vector<double>> xData, std::vector<double> yData, interpMethod method)
{

    xData_ = xData;
    yData_ = yData;
    method_  = method;

    nDim_ = xData_.size();

    xSize_ = std::vector<size_t>(nDim_, 0);

    xMin_ = std::vector<double>(nDim_, 0.0);
    xMax_ = std::vector<double>(nDim_, 0.0);

    iS_ = std::vector<size_t>(nDim_, 0);

    for (size_t iDim=0; iDim<nDim_; iDim++)
    {

        // Populate independent data dimensions
        xSize_[iDim] = xData_[iDim].size();

        // Get edge values
        xMin_[iDim] = *std::min_element(xData_[iDim].begin(), xData_[iDim].end());
        xMax_[iDim] = *std::max_element(xData_[iDim].begin(), xData_[iDim].end());

        // Initialize search indices
        size_t iLo = 0;
        size_t iHi = xSize_[iDim] - 1;
        iS_[iDim] = floor((iLo + iHi)/2);

    }

    switch (method_)
    {
        case LINEAR:
            init_linear();
            break;
        case PCHIP:
            init_pchip();
            break;
        case BILINEAR:
            init_bilinear();
            break;
    }

}

void Interp::check_method(std::string methodName, size_t methodDim)
{

    bool checkDim = (nDim_ == methodDim);

    size_t sizeTotal = 1;

    for (size_t iDim=0; iDim<nDim_; iDim++)
    {
        sizeTotal *= xSize_[iDim];
    }

    bool checkSize = (sizeTotal == yData_.size());

    if (!(checkDim && checkSize))
    {
        throw std::runtime_error("Input array dimensions incompatible with interp method: " + methodName);
    }

}

void Interp::init_linear()
{
    check_method("LINEAR", 1);
}

double Interp::update(double xq)
{
    double yq;
    switch (method_)
    {
        case LINEAR:
            yq = update_linear(xq);
            break;
        case PCHIP:
            yq = update_pchip(xq);
            break;
    }
    return yq;
}

double Interp::update(std::vector<double> xq)
{
    double yq;
    switch (method_)
    {
        case BILINEAR:
            yq = update_bilinear(xq);
            break;
    }
    return yq;
}

double Interp::update_linear(double xq)
{

    const size_t iDim = 0;
    double yq;

    if (xq <= xMin_[iDim]) // No extrapolation, use min y value
    {
        yq = yData_[0];
    }
    else if (xq >= xMax_[iDim]) // No extrapolation, use max y value
    {
        yq = yData_.back();
    }
    else // Evaluate on valid interval
    {

        // Get search index
        iS_[iDim] = search(xData_[iDim], xq, iS_[iDim]);

        // Perform linear interpolation
        double x0, x1, y0, y1, dx, dy;

        x0 = xData_[iDim][iS_[iDim]+0];
        x1 = xData_[iDim][iS_[iDim]+1];
        y0 = yData_      [iS_[iDim]+0];
        y1 = yData_      [iS_[iDim]+1];

        dx = x1 - x0;
        dy = y1 - y0;

        yq = y0 + (dy/dx)*(xq - x0);

    }

    return yq;

}

//---------------------------------------------------------------------------//

void Interp::init_pchip()
{

    check_method("PCHIP", 1);

    const std::vector<double>& x = xData_[0];
    const std::vector<double>& y = yData_;

    const size_t nX = xSize_[0];

    // 1. Get inter-element spacing (h) and linear slopes (del)
    pchip_.h.reserve(nX-1);
    pchip_.del.reserve(nX-1);

    for (size_t iX=0; iX<(nX-1); iX++)
    {
        pchip_.h[iX]   = x[iX+1] - x[iX];
        pchip_.del[iX] = (y[iX+1] - y[iX]) / pchip_.h[iX];
    }

    // 2. Compute derviative values for PCHIP method
    pchip_.d.reserve(nX);

    // 2A. Derivative at lower endpoint, use three-point method:
    //     Correct value to perserve shape if necessary
    pchip_.d[0] = deriv_three_point({x[0], x[1], x[2]}, {y[0], y[1], y[2]}, x[0]);

    if (sgn(pchip_.d[0]) != sgn(pchip_.del[0])) {pchip_.d[0] = 0.0;}

    // 2B. Derivative at upper endpoint, use three-point method:
    //     Correct value to perserve shape if necessary
    pchip_.d[nX-1] = deriv_three_point({x[nX-3], x[nX-2], x[nX-1]}, {y[nX-3], y[nX-2], y[nX-1]}, x[nX-1]);

    if (sgn(pchip_.d[nX-1]) != sgn(pchip_.del[nX-2])) {pchip_.d[nX-1] = 0.0;}

    // 2C. Derivatives at all other points:
    //     If signs of adjacent linear slopes (delta) don't match 
    //     or are zero, set derivative to zero;
    //     otherwise, if signs match compute derivative as weighted average
    double w1, w2;

    for (size_t iX=1; iX<(nX-1); iX++)
    {
        if (sgn(pchip_.del[iX])*sgn(pchip_.del[iX-1]) > 0)
        {
            w1    = 2*pchip_.h[iX] + pchip_.h[iX-1];
            w2    = pchip_.h[iX] + 2*pchip_.h[iX-1];
            pchip_.d[iX] = (w1 + w2)/(w1/pchip_.del[iX-1] + w2/pchip_.del[iX]);
        }
    }

}

double Interp::deriv_three_point(std::vector<double> x, std::vector<double> y, double xi)
{
    return   y[0]*((2*xi - x[1] - x[2])/((x[0] - x[1])*(x[0] - x[2]))) 
           + y[1]*((2*xi - x[0] - x[2])/((x[1] - x[0])*(x[1] - x[2]))) 
           + y[2]*((2*xi - x[0] - x[1])/((x[2] - x[0])*(x[2] - x[1])));
}

//---------------------------------------------------------------------------//

double Interp::update_pchip(double xq)
{

    const size_t iDim = 0;
    double yq;

    if (xq <= xMin_[iDim]) // No extrapolation, use min y value
    {
        yq = yData_[0];
    }
    else if (xq >= xMax_[iDim]) // No extrapolation, use max y value
    {
        yq = yData_.back();
    }
    else // Evaluate on valid interval
    {

        // Get search index
        iS_[iDim] = search(xData_[iDim], xq, iS_[iDim]);

        // Get coefficients
        const size_t iX = iS_[iDim];
        double s = xq - xData_[iDim][iX];

        double ck = (3*pchip_.del[iX] - 2*pchip_.d[iX] - pchip_.d[iX+1])/pchip_.h[iX];
        double bk = (pchip_.d[iX] - 2*pchip_.del[iX] + pchip_.d[iX+1])/pow(pchip_.h[iX], 2);

        // Power form interpolation
        yq = yData_[iX] + s*pchip_.d[iX] + pow(s, 2)*ck + pow(s, 3)*bk;

    }

    return yq;

}

//---------------------------------------------------------------------------//

void Interp::init_bilinear()
{
    check_method("BILINEAR", 2);
}

//---------------------------------------------------------------------------//

double Interp::update_bilinear(std::vector<double> xq)
{

    for (size_t iDim=0; iDim<nDim_; iDim++)
    {
        // No extrapolation; enforce limits on domain
        xq[iDim] = (xq[iDim] < xMin_[iDim]) ? xMin_[iDim] : xq[iDim];
        xq[iDim] = (xq[iDim] > xMax_[iDim]) ? xMax_[iDim] : xq[iDim];

        // Get search indices
        iS_[iDim] = search(xData_[iDim], xq[iDim], iS_[iDim]);
    }

    // Perform linear interpolation
    double x0, x1, y0, y1, dx, dy;
    std::array<double, 2> yq;

    for (size_t iDim=0; iDim<nDim_; iDim++)
    {

        x0 = xData_[0][iS_[0]+0];
        x1 = xData_[0][iS_[0]+1];
        y0 = yData_[get_2d_idx(iS_[0]+0, iS_[1]+iDim)];
        y1 = yData_[get_2d_idx(iS_[0]+1, iS_[1]+iDim)];

        dx = x1 - x0;
        dy = y1 - y0;

        yq[iDim] = y0 + (dy/dx)*(xq[0] - x0);

    }

    // Perform bilinear interpolation
    x0 = xData_[1][iS_[1]+0];
    x1 = xData_[1][iS_[1]+1];
    y0 = yq[0];
    y1 = yq[1];

    dx = x1 - x0;
    dy = y1 - y0;

    return y0 + (dy/dx)*(xq[1] - x0);

}

//---------------------------------------------------------------------------//

size_t Interp::get_2d_idx(size_t i0, size_t i1)
{
    return i0*xSize_[1] + i1;
}

//---------------------------------------------------------------------------//

size_t Interp::search(const std::vector<double>& x, double xq, size_t iS)
{

    /*
    Performs a binary search to find 
    the index of the first point in x
    that satisfies x[iS] <= xq
    
    Additional logic is used to accelerate the search
    using edge cases or the previous result
    */

    size_t iLo = 0;
    size_t iHi = x.size() - 1;

    // Check bounds

    if (xq == x[iLo])
    {
        iS = iLo;
        return iS;
    }
    else if (xq == x[iHi])
    {
        iS = iHi;
        return iS;
    }

    // Check immediately before and after current index

    if ((iS > iLo) && (xq >= x[iS-1]) && (xq < x[iS]))
    {
        return --iS;
    }
    else if ((iS < (iHi-1)) && (xq >= x[iS+1]) && (xq < x[iS+2]))
    {
        return ++iS;
    }

    // Finally, perform binary search

    while (true)
    {

        // xq < x[iLo+1]
        // goal: find index of (floor) closest x point)

        if ((xq >= x[iS]) && (xq < x[iS+1]))
            return iS;
        else
        {
            if (xq < x[iS])
            {
                iHi = iS;
            }
            else if (xq > x[iS])
            {
                iLo = iS;
            }
            iS = floor((iLo + iHi)/2);
        }

    }

}
