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

        void init(const std::string& outputDir, const std::string& metaStr, const int& nPrec);
        ~Telem();

        void update();
        void interp_boundary(std::string targetField, double targetPoint);
        void finalize();

        static std::vector<std::string> telemFields_;
        static std::vector<std::string> telemUnits_;

        stateMap state_;

    private:

        void init_output();
        void write_output();

        void update_stats();
        void write_stats();

        void finalize_output();

        telemArrayMap stateTelem_;
        telemMap      stateTelemMin_;
        telemMap      stateTelemMax_;

        std::string metaStr_;
        std::string outputDir_;
        std::string telemMode_;
        std::FILE*  telemFile_ {nullptr};

        uint8_t nPrec_ {0};
        size_t nTelemFields_ {0};
        size_t iStep_ {0};
        size_t iTelem_ {0};

        bool isInit_ {false};

};
