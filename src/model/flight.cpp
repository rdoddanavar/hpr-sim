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

void Flight::init(double t0Init, double dtInit, double tfInit)
{

    time = t0Init;
    dt   = dtInit;
    tf   = tfInit;

    nPrec = 3;

    massBody = 2.0;
    
    state = new stateMap;
    init_state(state);

    odeSolver.sys.function  = &ode_update;
    odeSolver.sys.jacobian  = nullptr;
    odeSolver.sys.dimension = 2;
    odeSolver.sys.params    = this;

    odeSolver.hStart = 1e-6;
    odeSolver.epsAbs = 1e-6;
    odeSolver.epsRel = 0.0;

    odeSolver.set_method("rkf45");

    odeSolver.driver = gsl_odeiv2_driver_alloc_y_new(&odeSolver.sys   ,
                                                      odeSolver.method,
                                                      odeSolver.hStart,
                                                      odeSolver.epsAbs,
                                                      odeSolver.epsRel);

    nStep = static_cast<int>(tf/dt);

    for (const auto& field : fields)
    {
        stateTelem[field] = std::vector<double>(nStep, 0.0);
    }

    isInit = true;

}

//---------------------------------------------------------------------------//

void Flight::set_state()
{
    state->emplace("time", &time);
    state->emplace("massBody", &massBody);
}

//---------------------------------------------------------------------------//

void Flight::update()
{

    int nStep = static_cast<int>(tf/dt);

    double y[] = {*state->at("linPosZ"),
                  *state->at("linVelZ")};

    // Save initial state
    update_deps();

    for (const auto& field : fields)
    {
        stateTelem[field][0] = *state->at(field);
    }

    // Solve ODE system

    for (int iStep = 1; iStep <= nStep; iStep++)
    {
        
        double ti = iStep*dt;

        int status = gsl_odeiv2_driver_apply(odeSolver.driver, &time, ti, y);

        for (const auto& field : fields)
        {
            stateTelem[field][iStep] = *state->at(field);
        }

        if (y[0] <= 0.0)
        {
            break;
        }

        /*
        if (status != GSL_SUCCESS)
        {
          printf ("error, return value=%d\n", status);
          break;
        }
        */
    }
}

//---------------------------------------------------------------------------//

int ode_update(double t, const double y[], double f[], void *params)
{
    
    Model*    flight = static_cast<Model*>(params);
    stateMap* state  = flight->state;

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

void Flight::write_telem(std::string fileOut) // maybe return bool for success/error status?
{

    std::ofstream ofs(fileOut);

    /*
    if (!ofs.is_open())
    {
        ; raise some error
    }
    */

    // Write data fields & units
    std::ostringstream oss;
    const char* delim = ",";

    std::copy(fields.begin(), fields.end() - 1, std::ostream_iterator<std::string>(oss, delim));
    oss << fields.back() << std::endl;

    std::copy(units.begin(), units.end() - 1, std::ostream_iterator<std::string>(oss, delim));
    oss << units.back() << std::endl;

    ofs << oss.str();

    // Write data values
    std::string strOut;

    for (int iStep = 0; iStep < nStep; iStep++)
    {
        
        oss.str("");
        oss.clear();

        for (const auto& field : fields)
        {
            oss << std::fixed << std::setprecision(nPrec) << stateTelem[field][iStep] << delim;
        }

        strOut = oss.str();
        strOut.pop_back();

        ofs << strOut << std::endl;

    }

}

//---------------------------------------------------------------------------//

Flight::~Flight()
{
    
    if (isInit)
    {
        delete state;
        gsl_odeiv2_driver_free(odeSolver.driver);
    }

}