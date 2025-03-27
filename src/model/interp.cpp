// System libraries
#include <cmath>
#include <algorithm>
#include <stdexcept>

// External libraries
#include "gsl/interpolation/gsl_interp.h"
#include "gsl/interpolation/gsl_interp2d.h"
#include "gsl/interpolation/gsl_spline.h"
#include "gsl/interpolation/gsl_spline2d.h"

// Project headers
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

void Interp::init(std::vector<std::vector<double>> dataInd, std::vector<double> dataDep, interpMethod method)
{

    dataInd_ = dataInd;
    dataDep_ = dataDep;
    method_  = method;

    nDim_ = dataInd_.size();

    xSize_ = std::vector<size_t>(nDim_, 0);

    xMin_ = std::vector<double>(nDim_, 0.0);
    xMax_ = std::vector<double>(nDim_, 0.0);

    iSearch_ = std::vector<size_t>(nDim_, 0);

    for (size_t iDim=0; iDim<nDim_; iDim++)
    {

        // Populate independent data dimensions
        xSize_[iDim] = dataInd_[iDim].size();

        // Get edge values
        xMin_[iDim] = *std::min_element(dataInd_[iDim].begin(), dataInd_[iDim].end());
        xMax_[iDim] = *std::max_element(dataInd_[iDim].begin(), dataInd_[iDim].end());

        // Initialize search indices
        size_t iLo = 0;
        size_t iHi = xSize_[iDim] - 1;
        iSearch_[iDim] = floor((iLo + iHi)/2);

    }

    switch (method_)
    {
        case LINEAR:
            init_linear();
            break;
        case PCHIP:
            //init_pchip();
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

    bool checkSize = (sizeTotal == dataDep_.size());

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
            //yq = update_pchip(xq[0]);
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
        yq = dataDep_[0];
    }
    else if (xq >= xMax_[iDim]) // No extrapolation, use max y value
    {
        yq = dataDep_.back();
    }
    else // Evaluate on valid interval
    {

        iSearch_[iDim] = search(dataInd_[iDim], xq, iSearch_[iDim]);

        double x0 = dataInd_[iDim][iSearch_[iDim]+0];
        double x1 = dataInd_[iDim][iSearch_[iDim]+1];
        double y0 = dataDep_      [iSearch_[iDim]+0];
        double y1 = dataDep_      [iSearch_[iDim]+1];

        double dx = x1 - x0;
        double dy = y1 - y0;

        // Perform linear interpolation
        yq = y0 + (dy/dx)*(xq - x0);

    }

    return yq;

}

//---------------------------------------------------------------------------//

/*

void Interp::init_pchip()
{
    ;
}

//---------------------------------------------------------------------------//

void Interp::update_phip()
{
    ;
}

*/

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
        iSearch_[iDim] = search(dataInd_[iDim], xq[iDim], iSearch_[iDim]);
    }

    // Perform bilinear interpolation
    double yq = 0.0;
    return yq;
}


//---------------------------------------------------------------------------//

size_t Interp::search(const std::vector<double>& x, double xq, size_t iSearch)
{

    // Performs a binary search to find 
    // the index of the first point in x
    // that satisfies x[i] <= xq;
    // Additional logic is used to accelerate the search
    // using edge cases or the previous result

    size_t iLo = 0;
    size_t iHi = x.size() - 1;

    // Check bounds

    if (xq == x[iLo])
    {
        iSearch = iLo;
        return iSearch;
    }
    else if (xq == x[iHi])
    {
        iSearch = iHi;
        return iSearch;
    }

    // Check immediately before and after current index

    if ((iSearch > iLo) && (xq >= x[iSearch-1]) && (xq < x[iSearch]))
    {
        return iSearch--;
    }
    else if ((iSearch < (iHi-1)) && (xq >= x[iSearch+1]) && (xq < x[iSearch+2]))
    {
        return iSearch++;
    }

    // Finally, perform binary search

    while (true)
    {

        // xq < x[iLo+1]
        // goal: find index of (floor) closest x point)

        if ((xq >= x[iSearch]) && (xq < x[iSearch+1]))
            return iSearch;
        else
        {
            if (xq < x[iSearch])
            {
                iHi = iSearch;
            }
            else if (xq > x[iSearch])
            {
                iLo = iSearch;
            }
            iSearch = floor((iLo + iHi)/2);
        }

    }

}
