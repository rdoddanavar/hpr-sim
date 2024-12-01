#pragma once

#include <cstdio>
#include <string>
#include <array>
#include <vector>
#include <unordered_map>
#include <memory>

// Macros
#define N_TELEM_ARRAY 1000
#define TELEM_TYPE    float

// Type aliases
using stateMap      = std::unordered_map<std::string, double*>;
using telemMap      = std::unordered_map<std::string, TELEM_TYPE>;
using telemArray    = std::array<TELEM_TYPE, N_TELEM_ARRAY>;
using telemArrayMap = std::unordered_map<std::string, telemArray>;

class Telem
{

    public:

        Telem(const std::string& telemModeIn,  const int& nPrecIn, const std::string& outputDirIn, const std::string& metaStrIn);
        ~Telem();

        void init();
        void init_output();
        void update(int iStep);
        void interp_boundary(std::string targetField, double targetPoint);
        void finalize(int iStep);

        static std::vector<std::string> telemFields;
        static std::vector<std::string> telemUnits;

        stateMap state;

    private:

        void init_output_text(const std::string& filePath);
        void init_output_binary(const std::string& filePath);

        void write_output();
        void write_output_text();
        void write_output_binary();

        void update_stats();
        void write_stats();

        void finalize_output_binary(int iStep);

        telemArrayMap stateTelem;
        telemMap      stateTelemMin;
        telemMap      stateTelemMax;

        std::string metaStr;
        std::string outputDir;
        std::string telemMode;
        std::FILE*  telemFile = nullptr;

        int nPrec;
        int nTelemFields;
        int iTelem;

};
