#pragma once

// System libraries
#include <string>
#include <vector>
#include <map>

class Model
{
    public: 

        // Data
        std::vector<Model*> depModels; // don't forget to use "new" keyword for mem alloc
        std::map<std::string, float> state;
        std::map<std::string, float> stateInit;

        // Function(s)
        virtual void initialize();
        void reset();
        virtual void update();
        void update_deps();

        // Constructor(s)
        Model();
};

class Engine : public Model
{
    public:

        // Data

        // Function(s)
        void initialize() override;
        void update() override;

        // Constructor(s)
        Engine();
};