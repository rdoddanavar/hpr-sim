// header

// System libraries

// Project libraries
#include "model.h"

// Model Class
void Model::reset()
{
    state = stateInit;
}

void Model::update_deps()
{

    /*
    Iterates over model dependencies, updates internal states 
    */

    for (auto dep : depModels)
    {
        dep->update();
    }
}
