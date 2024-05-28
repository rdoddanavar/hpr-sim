// System libraries

// External libraries

// Project headers
#include "model.h"

//---------------------------------------------------------------------------//

void Test::init(std::vector<std::string> stateFieldsInit)
{

    stateFields = stateFieldsInit;
    stateData   = std::vector<double>(stateFields.size());
    set_state_fields();

}

//---------------------------------------------------------------------------//

void Test::set_state_fields()
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

// TODO: overload this method to enable setting multiple fields
// void Test::set_state_data(std::vector<std::string> field, std::vector<double> data)

void Test::set_state_data(std::string field, double data)
{
    *state->at(field) = data;
}

//---------------------------------------------------------------------------//

// TODO: overload this method to enable getting multiple fields
// std::vector<double> Test::set_state_data(std::vector<std::string> field)

double Test::get_state_data(std::string field)
{
    return *state->at(field);
}
