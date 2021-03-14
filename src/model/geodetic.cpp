// System libraries
#include <cmath>

// External libraries
// <none>

// Project headers
#include "model.h"

//---------------------------------------------------------------------------//

void Geodetic::initialize(double phiInit) 
{

    // WGS 84 Constants
    gamE = 9.7803253359;        // [m/s^2]
    k    = 1.931852652458e-03;  // [-]
    e    = 8.1819190842622e-02; // [-]
    a    = 6378137.0;           // [m]
    f    = 3.3528106647475e-03; // [-]
    m    = 3.449786506841e-03;  // [-]

    phi = phiInit; // [rad]

    stateInit["gravity"] = wgs84(0.0);

    stateInit["linPosZ"] = 0.0; // remove later

    reset(); // Set state to IC's

}

//---------------------------------------------------------------------------//

void Geodetic::update(double timeEval)
{

    state["gravity"] = wgs84(state["linPosZ"]);

}

//---------------------------------------------------------------------------//

double Geodetic::wgs84(double h)
{
    
    // WGS 84 ELLIPSOIDAL GRAVITY FORMULA
    // NGA.STND.0036_1.0.0_WGS84

    /*
    Normal Gravity on the Ellipsoidal Surface (4-1)

    gam  = gamma; normal gravity at surface of ellipsoid
    gamE = normal gravity at the equator (on the ellispoid)
    k    = Somigliana’s Formula - normal gravity formula constant
    e    = first eccentricity of the ellispoid
    phi  = geodetic latitude
    */

    double sin2phi = pow(sin(phi), 2);
    double e2      = pow(e, 2);

    double gam = gamE * (1 + k*sin2phi) / sqrt(1 - e2*sin2phi);

    /*
    Normal Gravity Above the Ellipsoid - Taylor series expansion (4-3)

    gamH = gamma_h; normal gravity at height above ellipsoid
    a    = semi-major axis of the ellipsoid
    f    = WGS 84 flattening (reduced)
    m    = normal gravity formula constant (w^2*a^2*b/GM)
    h    = height above ellispoid
    */

    double a2 = pow(a, 2);
    double h2 = pow(h, 2);

    double gamH = gam * (1 - (2/a)*(1 + f + m - 2*f*sin2phi)*h + (3/a2)*h2);

    return gamH;

}