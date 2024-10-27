#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>

#include "fmt/core.h"
#include "fmt/ranges.h"

#include "telem.h"

//----------------------------------------------------------------------------//

Telem::Telem(const std::string& telemModeIn, const int& nPrecIn, const std::string& outputDirIn, const std::string& metaStrIn)
{

    // Telemetry setup
    telemMode = telemModeIn;
    nPrec     = nPrecIn;
    outputDir = outputDirIn;
    metaStr   = metaStrIn;

    nTelemFields = telemFields.size();

    for (const auto& field : telemFields)
    {
        stateTelem[field] = telemArray{0.0};
    }

    init_output();

}

//----------------------------------------------------------------------------//

void Telem::init()
{

    iTelem = 0;

    for (const auto& field : telemFields)
    {

        stateTelem[field][iTelem] = *state.at(field);

        stateTelemMin[field] = *state.at(field);
        stateTelemMax[field] = *state.at(field);

    }

}

void Telem::update(int iStep)
{

    iTelem = iStep % N_TELEM_ARRAY; // Index wraps from [0, N_TELEM_ARRAY]

    for (const auto& field : telemFields)
    {
        stateTelem[field][iTelem] = *state.at(field);
    }

    if (iTelem == (N_TELEM_ARRAY - 1))
    {
        write_output();
        update_stats();
    }

}

//----------------------------------------------------------------------------//

void Telem::finalize()
{

    // Finalize telem output
    write_output();

    // Finalize stats output
    update_stats();
    write_stats();

}

//----------------------------------------------------------------------------//

void Telem::init_output()
{

    std::string filePath = outputDir + "/telem";

    if (telemMode == "text")
    {
        filePath += ".csv";
        init_output_text(filePath);
    }
    else if (telemMode == "binary")
    {
        filePath += ".npy";
        init_output_binary(filePath);
    }

}

//----------------------------------------------------------------------------//

void Telem::init_output_text(const std::string& filePath)
{

    telemFile = std::fopen(filePath.c_str(), "w+");
    // TODO: Error catching here?

    // Write data fields & units
    auto out = fmt::memory_buffer();
    const char* delim = ",";

    fmt::format_to(std::back_inserter(out), "{}\n", metaStr);
    fmt::format_to(std::back_inserter(out), "{}\n", fmt::join(telemFields, delim));
    fmt::format_to(std::back_inserter(out), "{}\n", fmt::join(telemUnits , delim));
    fmt::print(telemFile, fmt::to_string(out));

}

void Telem::init_output_binary(const std::string& filePath)
{
    ;
}

//----------------------------------------------------------------------------//

void Telem::write_output()
{

    if (telemMode == "text")
    {
        write_output_text();
    }
    else if (telemMode == "binary")
    {
        write_output_binary();
    }

}

//----------------------------------------------------------------------------//

void Telem::write_output_text() // TODO: maybe return bool for success/error status?
{

    // Write data values
    auto out = fmt::memory_buffer();
    std::string delim;
    int iField;

    for (int iStep = 0; iStep < (iTelem + 1); iStep++)
    {

        iField = 0;

        for (const auto& field : telemFields)
        {
            delim = (++iField < nTelemFields) ? "," : "";
            fmt::format_to(std::back_inserter(out), "{:.{}f}{}", stateTelem[field][iStep], nPrec, delim);
        }

        fmt::format_to(std::back_inserter(out), "\n");

    }

    fmt::print(telemFile, fmt::to_string(out));

}

//----------------------------------------------------------------------------//


void Telem::write_output_binary()
{
    ;
}

//----------------------------------------------------------------------------//

void Telem::update_stats()
{
    double minValue;
    double maxValue;

    for (const auto& field : telemFields)
    {

        auto begin = stateTelem[field].begin();
        auto end   = begin + (iTelem + 1); // +1 for zero index

        minValue = *std::min_element(begin, end);
        maxValue = *std::max_element(begin, end);

        stateTelemMin[field] = std::min(stateTelemMin[field], minValue);
        stateTelemMax[field] = std::max(stateTelemMax[field], maxValue);

    } 
}

//----------------------------------------------------------------------------//

void Telem::write_stats()
{

    const std::string filePath  = outputDir + "/stats.yml";
    std::FILE*        statsFile = std::fopen(filePath.c_str(), "w+");
    // Error handling here?

    auto out = fmt::memory_buffer();
    const std::string tab = "    "; // 4 spaces

    // Document start
    fmt::format_to(std::back_inserter(out), "{}\n", metaStr);
    fmt::format_to(std::back_inserter(out), "---\n");

    std::string field;
    std::string units;

    for (int iField = 0; iField < nTelemFields; iField++)
    {

        field = telemFields[iField];
        units = telemUnits[iField];

        if (units.empty())
        {
            units = "null";
        }

       fmt::format_to(std::back_inserter(out), "{}:\n", field);
       fmt::format_to(std::back_inserter(out), "{}Units: {}\n", tab, units);
       fmt::format_to(std::back_inserter(out), "{}Min: {:.{}f}\n", tab, stateTelemMin[field], nPrec);
       fmt::format_to(std::back_inserter(out), "{}Max: {:.{}f}\n", tab, stateTelemMax[field], nPrec);

    }

    // Document end
    fmt::format_to(std::back_inserter(out), "...\n");
    fmt::print(statsFile, fmt::to_string(out));
    std::fclose(statsFile);

}

//----------------------------------------------------------------------------//

void Telem::interp_boundary(std::string targetField, double targetPoint)
{

    /*
    The last element in stateTelem["linPosZ"] is <= 0.0 (to trigger flight termination).
    A negative end position (in Z) is non-physical, i.e. under the ground.
    To correct the last data point, we find the zero point for linPosZ
    and linearly interpolate all other states to this zero point.

    Visualization:

    Index: [i1]------|------[i2]
    Value:  x1      0.0      x2
    */

    // Get indices of last two points
    int i2 = iTelem;
    int i1 = (i2 == 0) ? (N_TELEM_ARRAY - 1) : (i2 - 1);

    // Catching if end position is 0.0?

    double x1 = stateTelem[targetField][i1]; // End position prev (positive)
    double x2 = stateTelem[targetField][i2]; // End position      (negative)

    double dx12    = x1 - x2;          // Distance traveled between (positive)
    double dx10    = x1 - targetPoint; // Distance from zero point  (positive)
    double dx10Rel = dx10/dx12;        // " " normalized [0.0, 1.0]

    // Force linPosZ = 0.0 at flight termination
    stateTelem[targetField][i2] = targetPoint;

    for (const auto& field : telemFields)
    {

        if (field == targetField) continue;

        double y1 = stateTelem[field][i1]; // End state prev
        double y2 = stateTelem[field][i2]; // End state

        double dy12 = y2 - y1;

        // Perform interpolation
        double y0 = y1 + dx10Rel*dy12;

        // Overwrite last value
        stateTelem[field][i2] = y0;

    }

}

Telem::~Telem()
{
    std::fclose(telemFile);
}

//----------------------------------------------------------------------------//

std::vector<std::string> Telem::telemFields =
{
    "time"           ,
    "thrust"         ,
    "massEng"        ,
    "mass"           ,
    "gravity"        ,
    "temperature"    ,
    "pressure"       ,
    "density"        ,
    "dynamicPressure",
    "mach"           ,
    "alphaT"         ,
    "dragCoeff"      ,
    "dragForce"      ,
    "liftForce"      ,
    "forceZ"         ,
    "linAccZ"        ,
    "linVelZ"        ,
    "linPosZ"        ,
    "isBurnout"      ,
};

//----------------------------------------------------------------------------//

std::vector<std::string> Telem::telemUnits =
{
    "s"     ,
    "N"     ,
    "kg"    ,
    "kg"    ,
    "m/s^2" ,
    "K"     ,
    "Pa"    ,
    "kg/m^3",
    "Pa"    ,
    ""      ,
    "rad"   ,
    ""      ,
    "N"     ,
    "N"     ,
    "N"     ,
    "m/s^2" ,
    "m/s"   ,
    "m"     ,
    ""      ,
};
