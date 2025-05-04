// System headers
// <none>

// External headers
// <none>

// Internal headers
#include "interp.h"
#include "model.h"

//---------------------------------------------------------------------------//

void Engine::init(const numpyArray& timeArray  ,
                  const numpyArray& thrustArray,
                  const numpyArray& massArray  )
{

    std::vector<double> timeData   = process_numpy_array(timeArray);
    std::vector<double> thrustData = process_numpy_array(thrustArray);
    std::vector<double> massData   = process_numpy_array(massArray);

    thrustInterp_.init({timeData}, thrustData, Interp::PCHIP);
    massInterp_.init({timeData}, massData, Interp::PCHIP);

    thrust_  = thrustInterp_.update(0.0);
    massEng_ = massInterp_.update(0.0);

    timeMax_ = timeData.back();

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
