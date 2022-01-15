// System libraries

// External libraries

// Project headers
#include "model.h"

//---------------------------------------------------------------------------//

void Test::init(std::vector<std::string> stateFieldsInit)
{
    stateFields = stateFieldsInit;
}

//---------------------------------------------------------------------------//

void Test::set_state()
{

    for (const auto& field: stateFields)
    {
        state->emplace(field, new double); // TODO: do I need to delete this?
    }

    // TODO: problem --> state fields are not known at compile time

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