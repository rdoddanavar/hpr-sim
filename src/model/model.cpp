// header

// System libraries

// Project headers
#include "model.h"

// Model Class

Model::Model()
{
    ;
}

void Model::initialize()
{
    ; // set stateInit; set depModels in contructor
}

void Model::reset()
{
    state = stateInit;
}

void Model::update()
{
    ; // To be overridden in derived classes
}

void Model::update_deps()
{

    // Iterates over model dependencies, updates internal states 

    for (auto dep : depModels)
    {
        dep->update();
    }
}
