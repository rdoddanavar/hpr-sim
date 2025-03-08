#pragma once

#include <cstddef>
#include <vector>

using indvec = std::vector<std::vector<double>>;
using depvec = std::vector<double>;

enum interpMethod
{
    NONE     = 0, // TODO: NEAREST method?
    LINEAR   = 1,
    PCHIP    = 2,
    BILINEAR = 3
};

class Interp
{
    
    public:

        void init(indvec dataInd, depvec dataDep, interpMethod method);
        double update(std::vector<double> xq);

    private:

        void init_linear();
        double update_linear(std::vector<double> xq);
        void search(std::size_t iDim, double xq);

        indvec dataInd_;
        depvec dataDep_;

        interpMethod method_;

        std::size_t nDim_;

        std::vector<double> xMin_;
        std::vector<double> xMax_;

        std::vector<std::size_t> iSearch_;

};
