// System libraries
// <none>

// External libraries
#include "gsl/interpolation/gsl_interp.h"
#include "gsl/interpolation/gsl_spline.h"

// Project headers
#include "model.h"
#include "util_model.h"

//---------------------------------------------------------------------------//

void Engine::init(numpyArray& timeArray  ,
                  numpyArray& thrustArray,
                  numpyArray& massArray  )
{

    py::buffer_info timeBuff   = timeArray.request();
    py::buffer_info thrustBuff = thrustArray.request();
    py::buffer_info massBuff   = massArray.request();

    const size_t nTime = timeBuff.size;

    double* timePtr   = static_cast<double*>(timeBuff.ptr  );
    double* thrustPtr = static_cast<double*>(thrustBuff.ptr);
    double* massPtr   = static_cast<double*>(massBuff.ptr  );

    std::vector<std::vector<double>> timeData   = {std::vector<double>(timePtr  , timePtr   + timeBuff.size  )};
                std::vector<double>  thrustData =  std::vector<double>(thrustPtr, thrustPtr + thrustBuff.size);
                std::vector<double>  massData   =  std::vector<double>(massPtr  , massPtr   + massBuff.size  );

    thrustInterp_.init(timeData, thrustData, LINEAR);
    massInterp_.init(timeData, massData, LINEAR);

    thrust_  = thrustInterp_.update(0.0);
    massEng_ = massInterp_.update(0.0);

    timeMax_ = timeData[0][nTime-1];

    isInit_ = true;

}

//---------------------------------------------------------------------------//

void Engine::set_state_fields()
{

    state->emplace("thrust" , &thrust_);
    state->emplace("massEng", &massEng_);
    state->emplace("isBurnout", &isBurnout_);

}

//---------------------------------------------------------------------------//

void Engine::update()
{

    update_deps();

    double time = *state->at("time");

    if (!isBurnout_)
    {

        thrust_  = thrustInterp_.update(time);
        massEng_ = massInterp_.update(time);

        if (time >= timeMax_)
        {
            isBurnout_ = 1.0;
        }

    }
    else
    {
        thrust_ = 0.0;
    }

}
