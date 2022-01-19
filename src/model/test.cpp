// System libraries

// External libraries

// Project headers
#include "model.h"

//---------------------------------------------------------------------------//

void Test::init(std::vector<std::string> stateFieldsInit)
{
    
    stateFields = stateFieldsInit;
    stateData   = std::vector<double>(stateFields.size());
    set_state();

}

//---------------------------------------------------------------------------//

void Test::set_state()
{

    for (int i = 0; i < stateFields.size(); i++)
    {
        state->emplace(stateFields[i], &stateData[i]);
    }

}

//---------------------------------------------------------------------------//

void Test::update()
{
    update_deps();
}

//---------------------------------------------------------------------------//

void Test::set_state_data(std::string field, double data)
{
    *state->at(field) = data;
}

//---------------------------------------------------------------------------//

double Test::get_state_data(std::string field)
{
    return *state->at(field);
}