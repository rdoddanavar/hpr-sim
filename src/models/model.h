#ifndef MODEL_H
#define MODEL_H

// System libraries
#include <vector>
#include <map>

class Model
{
    public: 

        // Data
        std::vector<Model*> depModels; // don't forget to use "new" keyword for mem alloc
        std::vector<float> state;
        std::vector<float> stateInit;

        /*
        Use std::map instead for state container?
        std::map<std::string, float> state;
        */

        // Function(s)
        void initialize();
        void reset();
        void update();
        void update_deps();

        // Constructor(s)
};

#endif