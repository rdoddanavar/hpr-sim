// System libraries
#include <string>
#include <vector>
#include <map>

#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator>
#include <iomanip>

// External libraries
#include "gsl/ode-initval2/gsl_odeiv2.h"
#include "gsl/err/gsl_errno.h"

// Project headers
#include "model.h"
#include "util_model.h"

//---------------------------------------------------------------------------//

void Flight::init(double t0Init, double dtInit, double tfInit, int nPrecInit)
{
    
    // Basic setup
    time = t0Init;
    dt   = dtInit;
    tf   = tfInit;

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
    nStep = static_cast<int>(tf/dt);
    nPrec = nPrecInit;
    // TODO: Make nPrec an input parameter?

    // TODO: Initialize telemFields to default if user-defined fields not available

    for (const auto& field : telemFields)
    {
        stateTelem[field] = std::vector<double>(nStep, 0.0);
    }

    isInit = true;

}

//---------------------------------------------------------------------------//

void Flight::set_state()
{
    state->emplace("time", &time);
}

//---------------------------------------------------------------------------//

void Flight::update()
{

    int nStep = static_cast<int>(tf/dt);

    double y[] = {*state->at("linPosZ"),
                  *state->at("linVelZ")};

    // Save initial state
    update_deps();
    
    for (const auto& field : telemFields)
    {
        stateTelem[field][0] = *state->at(field);
    }
    
    // Solve ODE system

    for (int iStep = 1; iStep <= nStep; iStep++)
    {
        
        double ti = iStep*dt;

        int status = gsl_odeiv2_driver_apply(odeSolver.driver, &time, ti, y);

        update_deps(); // Reset state to correct time step

        for (const auto& field : telemFields)
        {
            stateTelem[field][iStep] = *state->at(field);
        }

        if (y[0] <= 0.0)
        {
            
            //flightTerm = true;
            flightTime = time;

            break; // TODO: could include more complex logic with an "apogeeFlag"
        }

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
    // TODO: Add additional while loop to keep integrating if flightTerm = false

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

std::vector<std::string> Flight::telemFieldsDefault = {"time"       ,
                                                       "thrust"     ,
                                                       "massEng"    ,
                                                       "mass"       ,
                                                       "gravity"    ,
                                                       "temperature",
                                                       "pressure"   ,
                                                       "density"    ,
                                                       "dynamicPressure",
                                                       "mach"       ,
                                                       "alphaT"     ,
                                                       "dragCoeff",
                                                       "dragForce"  ,
                                                       "liftForce"  ,
                                                       "forceZ"     ,
                                                       "linAccZ"    ,
                                                       "linVelZ"    ,
                                                       "linPosZ"    ,
                                                       "isBurnout"  };

std::vector<std::string> Flight::telemUnitsDefault = {"s"     ,
                                                      "N"     ,
                                                      "kg"    ,
                                                      "kg"    ,
                                                      "m/s^2" ,
                                                      "K"     ,
                                                      "Pa"    ,
                                                      "kg/m^3",
                                                      "Pa",
                                                      ""      ,
                                                      "rad"   ,
                                                      "",
                                                      "N"     ,
                                                      "N"     ,
                                                      "N"     ,
                                                      "m/s^2" ,
                                                      "m/s"   ,
                                                      "m"     ,
                                                      ""      };

std::vector<std::string> Flight::telemFields;
std::vector<std::string> Flight::telemUnits;

void Flight::set_telem(std::vector<std::string> telemFieldsInit)
{

    telemFields = telemFieldsInit;

    for (const auto& field : telemFields)
    {
        
        auto it  = std::find(telemFieldsDefault.begin(), telemFieldsDefault.end(), field);
        int  idx = std::distance(telemFieldsDefault.begin(), it);

        telemUnits.push_back(telemUnitsDefault[idx]);

    }

}

//---------------------------------------------------------------------------//

void Flight::write_telem(std::string fileOut) // TODO: maybe return bool for success/error status?
{

    if (!flightTerm)
    {
        // TODO: some kind of error message?
        return;
    }

    std::ofstream ofs;
    ofs.open(fileOut);

    /*
    if (!ofs.is_open())
    {
        ; TODO: raise some error
    }
    */

    // Write data fields & units
    std::ostringstream oss;
    const char* delim = ",";

    // TODO: consider replace std::endl w/ "\n" for performance

    std::copy(telemFields.begin(), telemFields.end() - 1, std::ostream_iterator<std::string>(oss, delim));
    oss << telemFields.back() << std::endl;

    std::copy(telemUnits.begin(), telemUnits.end() - 1, std::ostream_iterator<std::string>(oss, delim));
    oss << telemUnits.back() << std::endl;

    ofs << oss.str();

    // Write data values
    std::string strOut;

    for (int iStep = 0; iStep < nStep; iStep++)
    {
        
        oss.str("");
        oss.clear();

        for (const auto& field : telemFields)
        {
            oss << std::fixed << std::setprecision(nPrec) << stateTelem[field][iStep] << delim;
        }

        strOut = oss.str();
        strOut.pop_back();

        ofs << strOut << std::endl;

    }

    ofs.close();

}

//---------------------------------------------------------------------------//

void Flight::write_stats(std::string fileOut) // TODO: maybe return bool for success/error status?
{

    if (!flightTerm)
    {
        // TODO: some kind of error message?
        return;
    }

    std::ofstream ofs;
    ofs.open(fileOut);

    /*
    if (!ofs.is_open())
    {
        ; TODO: raise some error
    }
    */

    // Flight Time
    //ofs << "Flight Time: " << std::fixed << std::setprecision(nPrec) << flightTime << std::endl;

    // Max values
    double maxValue;

    for (const auto& field : telemFields)
    {
        
        maxValue = *std::max_element(stateTelem[field].begin(), stateTelem[field].end());
        
        ofs << "(MAX) " << field << ": ";
        ofs << std::fixed << std::setprecision(nPrec) << maxValue << std::endl;

    }

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