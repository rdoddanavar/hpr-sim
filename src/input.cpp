// Installed libraries
#include <string>

// Project libraries
#include "input.h"

// Member function definition(s)
    
Param::Param() : value(0.0), min(0.0), max(0.0), unit("") { }

Param::Param(float inp) : min(0.0), max(0.0), unit("") 
{ 
    value = inp; 
}