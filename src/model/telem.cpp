#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>

#include "fmt/core.h"
#include "fmt/ranges.h"

#include "telem.h"

//----------------------------------------------------------------------------//

void Telem::init(const std::string& outputDir, const std::string& metaStr, const int& nPrec)
{

    if (!isInit_)
    {

        // Telemetry setup
        outputDir_ = outputDir;
        metaStr_   = metaStr;
        nPrec_     = nPrec;

        nTelemFields_ = telemFields_.size();

        for (const auto& field : telemFields_)
        {
            stateTelem_[field] = telemArray{static_cast<TELEM_TYPE>(0.0)};
        }

        init_output();

        iStep_  = 0;
        iTelem_ = 0;
        isInit_ = true;

    }
    else
    {
        throw std::runtime_error("Telem object already initialized");
    }

}

//----------------------------------------------------------------------------//

void Telem::init_output()
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

    const std::string filePath = outputDir_ + "/telem.npy";
    telemFile_ = std::fopen(filePath.c_str(), "wb");

    const size_t nHdr = 128; // No. bytes
    std::array<uint8_t, nHdr> header{0x00}; // Initialize header with NULL

    // Initialize header bytes; Why? Array size not known ahead of time
    // After array data is written, determine array shape and overwrite header w/ correct metadata
    std::fwrite(header.data(), 1, nHdr, telemFile_);

}

//----------------------------------------------------------------------------//

void Telem::update()
{

    if (isInit_)
    {

        iTelem_ = iStep_ % N_TELEM_ARRAY; // Index wraps from [0, N_TELEM_ARRAY]

        for (const auto& field : telemFields_)
        {
            stateTelem_[field][iTelem_] = static_cast<TELEM_TYPE>(*state_.at(field));
        }

        if (iTelem_ == (N_TELEM_ARRAY - 1))
        {
            write_output();
            update_stats();
        }
        else if (iTelem_ == 0)
        {
            for (const auto& field : telemFields_)
            {
                // Initialize minmax stats to current field values
                stateTelemMin_[field] = static_cast<TELEM_TYPE>(*state_.at(field));
                stateTelemMax_[field] = static_cast<TELEM_TYPE>(*state_.at(field));
            }
        }
        iStep_++;
    }
    else
    {
        throw std::runtime_error("Telem object not initialized");
    }

}

//----------------------------------------------------------------------------//

void Telem::update_stats()
{

    TELEM_TYPE minValue;
    TELEM_TYPE maxValue;

    for (const auto& field : telemFields_)
    {

        auto begin = stateTelem_[field].begin();
        auto end   = begin + (iTelem_ + 1); // +1 for zero index

        minValue = static_cast<TELEM_TYPE>(*std::min_element(begin, end));
        maxValue = static_cast<TELEM_TYPE>(*std::max_element(begin, end));

        stateTelemMin_[field] = std::min(stateTelemMin_[field], minValue);
        stateTelemMax_[field] = std::max(stateTelemMax_[field], maxValue);

    }

}

//----------------------------------------------------------------------------//


void Telem::write_output()
{

    // See Telem::init_output_binary for NPY file format

    // Write data values
    std::vector<TELEM_TYPE> out(nTelemFields_, 0.0);
    size_t iField;

    for (size_t iArr = 0; iArr < (iTelem_ + 1); iArr++)
    {

        iField = 0;

        for (const auto& field : telemFields_)
        {
            out[iField] = stateTelem_[field][iArr];
            iField += 1;
        }

        std::fwrite(out.data(), sizeof(TELEM_TYPE), nTelemFields_, telemFile_);

    }

}

//----------------------------------------------------------------------------//

void Telem::write_stats()
{

    const std::string filePath  = outputDir_ + "/stats.yml";
    std::FILE*        statsFile = std::fopen(filePath.c_str(), "w");
    // Error handling here?

    auto out = fmt::memory_buffer();
    const std::string tab = "    "; // 4 spaces

    // Document start
    fmt::format_to(std::back_inserter(out), "{}\n", metaStr_);
    fmt::format_to(std::back_inserter(out), "---\n");

    std::string field;
    std::string unit;

    for (int iField = 0; iField < nTelemFields_; iField++)
    {

        field = telemFields_[iField];
        unit  = telemUnits_[iField];

        if (unit.empty())
        {
            unit = "null";
        }

       fmt::format_to(std::back_inserter(out), "{}:\n", field);
       fmt::format_to(std::back_inserter(out), "{}unit: {}\n", tab, unit);
       fmt::format_to(std::back_inserter(out), "{}min: {:.{}f}\n", tab, stateTelemMin_[field], nPrec_);
       fmt::format_to(std::back_inserter(out), "{}max: {:.{}f}\n", tab, stateTelemMax_[field], nPrec_);

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
    int i2 = iTelem_;
    int i1 = (i2 == 0) ? (N_TELEM_ARRAY - 1) : (i2 - 1);

    // Catching if end position is 0.0?

    double x1 = stateTelem_[targetField][i1]; // End position prev (positive)
    double x2 = stateTelem_[targetField][i2]; // End position      (negative)

    double dx12    = x1 - x2;          // Distance traveled between (positive)
    double dx10    = x1 - targetPoint; // Distance from zero point  (positive)
    double dx10Rel = dx10/dx12;        // " " normalized [0.0, 1.0]

    // Force linPosZ = 0.0 at flight termination
    stateTelem_[targetField][i2] = targetPoint;

    for (const auto& field : telemFields_)
    {

        if (field == targetField) continue;

        double y1 = stateTelem_[field][i1]; // End state prev
        double y2 = stateTelem_[field][i2]; // End state

        double dy12 = y2 - y1;

        // Perform interpolation
        double y0 = y1 + dx10Rel*dy12;

        // Overwrite last value
        stateTelem_[field][i2] = y0;

    }

}

//----------------------------------------------------------------------------//

void Telem::finalize()
{

    if (isInit_)
    {

        // Finalize telem output
        write_output();
        finalize_output();

        // Finalize stats output
        update_stats();
        write_stats();

        isInit_ = false;

    }
    else
    {
        throw std::runtime_error("Telem object not initialized");
    }

}

//----------------------------------------------------------------------------//

void Telem::finalize_output()
{

    // See Telem::init_output_binary for NPY file format

    const size_t nHdr = 128; // No. bytes
    const size_t nMag = 6;   //
    const size_t nVer = 2;   // ""
    const size_t nLen = 2;   // ""

    std::array<uint8_t, nMag> magic   = {0x93, 'N', 'U', 'M', 'P', 'Y'};
    std::array<uint8_t, nVer> version = {0x01, 0x00};
    std::array<uint16_t, 1>   dataLen = {0x0076};

    // Go back and populate header
    std::rewind(telemFile_);

    // 1. Magic string
    std::fwrite(magic.data(), 1, nMag, telemFile_); // nMag elements, 1 byte each

    // 2. & 3. Format version
    std::fwrite(version.data(), 1, nVer, telemFile_); // nVer elements, 1 byte each

    // 4. Header data length
    std::fwrite(dataLen.data(), nLen, 1, telemFile_); // 1 element, nLen bytes each

    // 5. Python dict string
    // Example: "{'descr': '<f8', 'fortran_order': False, 'shape': (2, 2), }"
    uint32_t nRow = static_cast<uint32_t>(iStep_); // iStep_ will correspond to number of rows; see update()
    uint32_t nCol = static_cast<uint32_t>(nTelemFields_);

    std::string dictStr = fmt::format("{{'descr': '<f{:d}', 'fortran_order': False, 'shape': ({:d}, {:d}), }}",
                                      sizeof(TELEM_TYPE), nRow, nCol);
    size_t nDict = dictStr.length();

    std::fwrite(dictStr.data(), 1, nDict, telemFile_);

    // 6. Padding
    const size_t nPad = nHdr - (nMag + nVer + nLen + nDict);
    std::vector<uint8_t> space(nPad, 0x20);
    space[nPad-1] = 0x0A; // Set let element as new line
    std::fwrite(space.data(), 1, nPad, telemFile_);

    // 7. Cleanup
    std::fclose(telemFile_);

}

//----------------------------------------------------------------------------//

Telem::~Telem()
{
    if (isInit_)
    {
        finalize();
    }
}

//----------------------------------------------------------------------------//

std::vector<std::string> Telem::telemFields_ =
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

std::vector<std::string> Telem::telemUnits_ =
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
