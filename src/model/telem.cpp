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
        stateTelem[field] = telemArray{static_cast<TELEM_TYPE>(0.0)};
    }

    init_output();

}

//----------------------------------------------------------------------------//

void Telem::init()
{

    iTelem = 0;

    for (const auto& field : telemFields)
    {

        stateTelem[field][iTelem] = static_cast<TELEM_TYPE>(*state.at(field));

        stateTelemMin[field] = static_cast<TELEM_TYPE>(*state.at(field));
        stateTelemMax[field] = static_cast<TELEM_TYPE>(*state.at(field));

    }

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

    telemFile = std::fopen(filePath.c_str(), "w");
    // TODO: Error catching here?

    // Write data fields & units
    auto out = fmt::memory_buffer();
    const char* delim = ",";

    fmt::format_to(std::back_inserter(out), "{}\n", metaStr);
    fmt::format_to(std::back_inserter(out), "{}\n", fmt::join(telemFields, delim));
    fmt::format_to(std::back_inserter(out), "{}\n", fmt::join(telemUnits , delim));
    fmt::print(telemFile, fmt::to_string(out));

}

//----------------------------------------------------------------------------//

void Telem::init_output_binary(const std::string& filePath)
{

    /*
    NPY Format v1.0
    https://numpy.org/devdocs/reference/generated/numpy.lib.format.html

    1. 6 bytes : Magic string = 0x93 + "NUMPY"
    2. 1 byte  : Major version = 0x01
    3. 1 byte  : Minor version = 0x00
    4. 2 bytes : Length of header data (uint16 LE)
    5  D bytes : ACII string (python dict) describing array's format
    6. P bytes : Padding w/ spaces (0x20) ending with newline '\n' (0x0A)
    7. A bytes : Floating point array data (row-major)

    Total bytes in header = 128
    Length of header data = D + P = 128 - (6+1+1+2) = 118
    Length of padding     = 128 - (6+1+1+2) - D
    Length of array data  = A*4 (float) OR A*8 (double)
    */

    //------------------------------------------------------------------------//

    telemFile = std::fopen(filePath.c_str(), "wb");

    const std::size_t nHdr = 128; // No. bytes
    std::array<uint8_t, nHdr> header{0x00}; // Initialize header with NULL

    // Initialize header bytes; Why? Array size not known ahead of time
    // After array data is written, determine array shape and overwrite header w/ correct metadata
    std::fwrite(header.data(), 1, nHdr, telemFile);

}

//----------------------------------------------------------------------------//

void Telem::update(int iStep)
{

    iTelem = iStep % N_TELEM_ARRAY; // Index wraps from [0, N_TELEM_ARRAY]

    for (const auto& field : telemFields)
    {
        stateTelem[field][iTelem] = static_cast<TELEM_TYPE>(*state.at(field));
    }

    if (iTelem == (N_TELEM_ARRAY - 1))
    {
        write_output();
        update_stats();
    }

}

//----------------------------------------------------------------------------//

void Telem::update_stats()
{

    TELEM_TYPE minValue;
    TELEM_TYPE maxValue;

    for (const auto& field : telemFields)
    {

        auto begin = stateTelem[field].begin();
        auto end   = begin + (iTelem + 1); // +1 for zero index

        minValue = static_cast<TELEM_TYPE>(*std::min_element(begin, end));
        maxValue = static_cast<TELEM_TYPE>(*std::max_element(begin, end));

        stateTelemMin[field] = std::min(stateTelemMin[field], minValue);
        stateTelemMax[field] = std::max(stateTelemMax[field], maxValue);

    }

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

    // See Telem::init_output_binary for NPY file format

    // Write data values
    std::vector<TELEM_TYPE> out(nTelemFields, 0.0);
    int iField;

    for (int iStep = 0; iStep < (iTelem + 1); iStep++)
    {

        iField = 0;

        for (const auto& field : telemFields)
        {
            out[iField] = stateTelem[field][iStep];
            iField += 1;
        }

        std::fwrite(out.data(), sizeof(TELEM_TYPE), nTelemFields, telemFile);

    }

}

//----------------------------------------------------------------------------//

void Telem::write_stats()
{

    const std::string filePath  = outputDir + "/stats.yml";
    std::FILE*        statsFile = std::fopen(filePath.c_str(), "w");
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

//----------------------------------------------------------------------------//

void Telem::finalize(int iStep)
{

    // Finalize telem output
    if (telemMode == "text")
    {
        write_output();
    }
    else if (telemMode == "binary")
    {
        write_output();
        finalize_output_binary(iStep);
    }

    // Finalize stats output
    update_stats();
    write_stats();

}

//----------------------------------------------------------------------------//

void Telem::finalize_output_binary(int iStep)
{

    // See Telem::init_output_binary for NPY file format

    const std::size_t nHdr = 128; // No. bytes
    const std::size_t nMag = 6;   //
    const std::size_t nVer = 2;   // ""
    const std::size_t nLen = 2;   // ""

    std::array<uint8_t, nMag> magic = {0x93, 'N', 'U', 'M', 'P', 'Y'};
    std::array<uint8_t, nVer> version = {0x01, 0x00};
    std::array<std::uint16_t, 1> dataLen = {0x0076};

    // Go back and populate header
    std::rewind(telemFile);

    // 1. Magic string
    std::fwrite(magic.data(), 1, nMag, telemFile); // nMag elements, 1 byte each

    // 2. & 3. Format version
    std::fwrite(version.data(), 1, nVer, telemFile); // nVer elements, 1 byte each

    // 4. Header data length
    std::fwrite(dataLen.data(), nLen, 1, telemFile); // 1 element, nLen bytes each

    // 5. Python dict string
    // Example: "{'descr': '<f8', 'fortran_order': False, 'shape': (2, 2), }"
    uint32_t nRow = static_cast<uint32_t>(iStep + 1);
    uint32_t nCol = static_cast<uint32_t>(nTelemFields);

    std::string dictStr = fmt::format("{{'descr': '<f{:d}', 'fortran_order': False, 'shape': ({:d}, {:d}), }}",
                                      sizeof(TELEM_TYPE), nRow, nCol);
    std::size_t nDict = dictStr.length();

    std::fwrite(dictStr.data(), 1, nDict, telemFile);

    // 6. Padding
    const std::size_t nPad = nHdr  - (nMag + nVer + nLen + nDict);
    std::vector<uint8_t> space(nPad, 0x20);
    space[nPad-1] = 0x0A; // Set let element as new line
    std::fwrite(space.data(), 1, nPad, telemFile);

}

//----------------------------------------------------------------------------//

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
