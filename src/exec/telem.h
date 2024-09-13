#include <cstdio>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

// Macros
#define N_TELEM_ARRAY 1000

// Type aliases
using stateMap      = std::unordered_map<std::string, double*>;
using stateMapPtr   = std::shared_ptr<stateMap>;

using telemMap      = std::unordered_map<std::string, double>;
using telemArray    = std::array<double, N_TELEM_ARRAY>;
using telemArrayMap = std::unordered_map<std::string, telemArray>;

class Telem
{

    public:

        Telem(const std::string& telemModeIn,  const int& nPrecIn, const std::string& outputDirIn, const std::string& metaStrIn);

        void set_telem_fields(const std::vector<std::string>& telemFieldsInit, const std::vector<std::string>& telemUnitsInit);
        void init();
        void update(int iStep);
        void interp_boundary(std::string targetField, double targetPoint);
        void finalize();

        void init_output();

        static std::vector<std::string> telemFieldsDefault;
        static std::vector<std::string> telemUnitsDefault;

        stateMap state;

    private:

        void init_output_text(const std::string& filePath);
        void init_output_binary(const std::string& filePath);

        void write_output_text();
        void write_output_binary();

        void write_output();
        void update_stats();
        void write_stats();

        std::string outputDir;

        std::vector<std::string> telemFields;
        std::vector<std::string> telemUnits;

        int nTelemFields;

        telemArrayMap stateTelem;
        telemMap      stateTelemMin;
        telemMap      stateTelemMax;

        std::string   telemMode;
        std::FILE*    telemFile;

        std::string metaStr;

        int    nPrec;
        int    iTelem;

};
