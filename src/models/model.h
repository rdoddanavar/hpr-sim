#ifndef MODEL_H
#define MODEL_H

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
        void initialize();
        void reset();
        void update();
        void update_deps();

        // Constructor(s)
};

#endif