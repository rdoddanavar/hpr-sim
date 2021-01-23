#pragma once

// System libraries
#include <string>
#include <vector>
#include <map>
#include <cstdio>

// Project headers
#include "pybind11/pybind11.h"

namespace py = pybind11;

// Class definitions

class Model
{
    public: 

        // Data
        //std::vector<Model*> depModels; // don't forget to use "new" keyword for mem alloc
        //std::map<std::string, float> state;
        //std::map<std::string, float> stateInit;

        // Function(s)
        virtual void initialize();
        void test()
        {
            printf("test\n");
        };
        
        
        // ONLY COMPILES IF DEFINED IN CLASS!!! --> that doesn't make sense; unit test is fine with this
        // Error only exists for separate file!
        // Consider abstract class? methods?
        
        //{
        //    printf("hello\n");
        //};
        //void reset();
        //virtual void update();
        //void update_deps();

        // Constructor(s)
        //Model();
};

/*
Model::Model()
{
    ;
}
*/

void Model::initialize()
{
    printf("hello\n"); // set stateInit; set depModels in contructor
}

/*
void Model::reset()
{
    state = stateInit;
}
*/

/*
void Model::update()
{
    ; // To be overridden in derived classes
}
*/

/*
void Model::update_deps()
{

    // Iterates over model dependencies, updates internal states 

    for (auto dep : depModels)
    {
        dep->update();
    }
}
*/

// Binding code

void init_Engine(py::module_ &);

PYBIND11_MODULE(model, m)
{
    
    m.doc() = "Simulation Model Classes"; // Optional module docstring

    // Exposing base class necessary for dervied construction
    // Base methods exposed once, automatically available to dervied in python
    py::class_<Model>(m, "Model")
        .def(py::init<>())
        .def("test", &Model::test);

    init_Engine(m);

}