// System libraries
#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <iomanip>

// External libraries
#include "gsl/ode-initval2/gsl_odeiv2.h"
#include "gsl/err/gsl_errno.h"
#include "fmt/core.h"
#include "fmt/ranges.h"

// Project headers
#include "model.h"
#include "util_model.h"

//---------------------------------------------------------------------------//

void Flight::init(const std::string& telemModeIn,  const int& nPrecIn, const std::string& outputDir)
{

    // Solver setup
    odeSolver.sys.function  = &ode_update;
    odeSolver.sys.jacobian  = nullptr;
    odeSolver.sys.dimension = 2;
    odeSolver.sys.params    = this;

    odeSolver.hStart = 1e-6;
    odeSolver.epsAbs = 1e-9;
    odeSolver.epsRel = 1e-9;

    odeSolver.set_method("rkf45");

    odeSolver.driver = gsl_odeiv2_driver_alloc_y_new(&odeSolver.sys   ,
                                                      odeSolver.method,
                                                      odeSolver.hStart,
                                                      odeSolver.epsAbs,
                                                      odeSolver.epsRel);

    // Telemetry setup
    telemMode = telemModeIn;
    nPrec     = nPrecIn;

    init_telem(outputDir);

    telemFields = telemFieldsDefault;
    telemUnits  = telemUnitsDefault;

    nTelemFields = telemFields.size();

    for (const auto& field : telemFields)
    {
        stateTelem[field] = telemArray{0.0f};
    }

    isInit = true;

}

//---------------------------------------------------------------------------//

void Flight::set_state_fields()
{
    state->emplace("time", &time);
}

//---------------------------------------------------------------------------//

void Flight::update()
{

    dt = 0.01;
    int iStep = 0;
    int iTelem = 0;

    double y[] = {*state->at("linPosZ"),
                  *state->at("linVelZ")};

    // Save initial state
    update_deps();

    for (const auto& field : telemFields)
    {

        stateTelem[field][iTelem] = static_cast<float>(*state->at(field));

        stateTelemMin[field] = *state->at(field);
        stateTelemMax[field] = *state->at(field);

    }

    iStep += 1;

    // Solve ODE system

    while (y[0] >= 0.0)
    {

        double ti = iStep*dt;

        int status = gsl_odeiv2_driver_apply(odeSolver.driver, &time, ti, y);

        update_deps(); // Reset state to correct time step

        iTelem = iStep % N_TELEM_ARRAY; // Index wraps from [0, N_TELEM_ARRAY]

        for (const auto& field : telemFields)
        {
           stateTelem[field][iTelem] = static_cast<float>(*state->at(field));
        }

        if (iTelem == (N_TELEM_ARRAY - 1))
        {
            write_telem(iTelem);
            update_stats(iTelem);
        }

        iStep += 1;

        // TODO: could include more complex logic with an "apogeeFlag"

        /*
        TODO
        if (status != GSL_SUCCESS)
        {
          printf ("error, return value=%d\n", status);
          break;
        }
        */

    }

    flightTerm = true; // TODO: better handling for flight termination
    // interpolate_state();
    // missing last telem elements???
    write_telem(iTelem);
    fclose(telemFile);
    update_stats(iTelem);

}

//---------------------------------------------------------------------------//

int ode_update(double t, const double y[], double f[], void *params)
{

    Model*      flight = static_cast<Model*>(params);
    stateMapPtr state  = flight->state;

    // Set current state
    *state->at("linPosZ") = y[0];
    *state->at("linVelZ") = y[1];

    flight->update_deps();

    // Set state derivatives for solver
    f[0] = y[1];
    f[1] = *state->at("linAccZ");

    return GSL_SUCCESS;

}

//---------------------------------------------------------------------------//

void Flight::set_telem(const std::vector<std::string>& telemFieldsInit, const std::vector<std::string>& telemUnitsInit)
{
    telemFields = telemFieldsInit;
    telemUnits  = telemUnitsInit;
}

//---------------------------------------------------------------------------//

std::vector<std::string> Flight::telemFieldsDefault =
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

std::vector<std::string> Flight::telemUnitsDefault =
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

//---------------------------------------------------------------------------//

void Flight::init_telem(const std::string& outputDir)
{

    std::string filePath = outputDir + "/telem";

    if (telemMode == "text")
    {
        filePath += ".csv";
        init_telem_text(filePath);
    }
    else if (telemMode == "binary")
    {
        filePath += ".npy";
        init_telem_binary(filePath);
    }

}

//---------------------------------------------------------------------------//

void Flight::init_telem_text(const std::string& filePath)
{

    telemFile = std::fopen(filePath.c_str(), "w+");
    // TODO: Error catching here?

    // Write data fields & units
    auto out = fmt::memory_buffer();
    const char* delim = ",";

    fmt::format_to(std::back_inserter(out), "{}\n", fmt::join(telemFields, delim));
    fmt::format_to(std::back_inserter(out), "{}\n", fmt::join(telemUnits , delim));
    fmt::print(telemFile, fmt::to_string(out));

}

//---------------------------------------------------------------------------//

void Flight::init_telem_binary(const std::string& filePath)
{
    ;
}

//---------------------------------------------------------------------------//

void Flight::write_telem(const int& iTelem)
{

    if (telemMode == "text")
    {
        write_telem_text(iTelem);
    }
    else if (telemMode == "binary")
    {
        write_telem_binary(iTelem);
    }

}

//---------------------------------------------------------------------------//

void Flight::write_telem_text(const int& iTelem) // TODO: maybe return bool for success/error status?
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
            iField++;
            delim = (iField < nTelemFields) ? "," : "";
            fmt::format_to(std::back_inserter(out), "{:.{}f}{}", stateTelem[field][iStep], nPrec, delim);
        }

        fmt::format_to(std::back_inserter(out), "\n");

    }

    fmt::print(telemFile, fmt::to_string(out));

}

//---------------------------------------------------------------------------//

void Flight::write_telem_binary(const int& iTelem)
{
    ;
}

//---------------------------------------------------------------------------//

void Flight::update_stats(const int& iTelem)
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

//---------------------------------------------------------------------------//

void Flight::write_stats(const std::string& filePath) // TODO: maybe return bool for success/error status?
{

    std::ofstream ofs;
    ofs.open(filePath, std::ofstream::trunc);

    /*
    if (!ofs.is_open())
    {
        ; TODO: raise some error
    }
    */

    // Document start
    ofs << "---\n";

    std::string tab = "    ";

    int nField = telemFields.size();
    std::string field;
    std::string units;

    for (int iField = 0; iField < nField; iField++)
    {

        field = telemFields[iField];
        units = telemUnits[iField];

        if (units.empty())
        {
            units = "null";
        }

        ofs << field << ":\n";
        ofs << tab << "Units: " << units << "\n";
        ofs << tab << "Min: " << std::fixed << std::setprecision(nPrec) << stateTelemMin[field] << "\n";
        ofs << tab << "Max: " << std::fixed << std::setprecision(nPrec) << stateTelemMax[field] << "\n";

    }

    // Document end
    ofs << "...\n";
    ofs.close();

}

//---------------------------------------------------------------------------//

Flight::~Flight()
{

    if (isInit)
    {
        gsl_odeiv2_driver_free(odeSolver.driver);
    }

}
