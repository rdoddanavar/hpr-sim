// System libraries
#include <cmath>

// External libraries
// <none>

// Project headers
#include "model.h"

//---------------------------------------------------------------------------//

// WGS 84 Constants
const double gammaE = 9.7803253359;        // [m/s^2]
const double k      = 1.931852652458e-03;  // [-]
const double e      = 8.1819190842622e-02; // [-]
const double a      = 6378137.0;           // [m]
const double f      = 3.3528106647475e-03; // [-]
const double m      = 3.449786506841e-03;  // [-]

//---------------------------------------------------------------------------//

void Geodetic::init(double phiInit, double altInit) 
{

    phi = phiInit; // [rad]
    wgs84_init();
    gravity0 = gamma;

    altitudeMSL0 = altInit;      // [m]
    altitudeMSL  = altitudeMSL0; // [m]
    altitudeAGL  = 0.0;          // [m]
    wgs84();

    isInit = true;

}

//---------------------------------------------------------------------------//

void Geodetic::set_state()
{

    state->emplace("altitudeMSL", &altitudeMSL);
    state->emplace("altitudeAGL", &altitudeAGL);
    state->emplace("gravity", &gravity);
    state->emplace("gravity0", &gravity0);

}

//---------------------------------------------------------------------------//

void Geodetic::update()
{

    update_deps();

    altitudeAGL = *state->at("linPosZ");
    altitudeMSL = altitudeMSL0 + altitudeAGL;
    wgs84();

}

//---------------------------------------------------------------------------//

void Geodetic::wgs84_init()
{

    /*
    NGA.STND.0036_1.0.0_WGS84
    Normal Gravity on the Ellipsoidal Surface (4-1)

    gamma  = normal gravity at surface of ellipsoid
    gammaE = normal gravity at the equator (on the ellispoid)
    k      = Somigliana’s Formula - normal gravity formula constant
    e      = first eccentricity of the ellispoid
    phi    = geodetic latitude
    */

   // TODO: calculate latitude drift with rocket drift downrange?
   // This would require the subroutine to be folded back into wgs84()

    sin2phi   = pow(sin(phi), 2);
    double e2 = pow(e, 2);

    gamma = gammaE * (1 + k*sin2phi) / sqrt(1 - e2*sin2phi);

}

//---------------------------------------------------------------------------//

void Geodetic::wgs84()
{

    /*
    NGA.STND.0036_1.0.0_WGS84
    Normal Gravity Above the Ellipsoid - Taylor series expansion (4-3)

    gamma  = normal gravity at surface of ellipsoid
    gammaH = normal gravity at height above ellipsoid
    a      = semi-major axis of the ellipsoid
    f      = WGS 84 flattening (reduced)
    m      = normal gravity formula constant (w^2*a^2*b/GM)
    h      = height above ellispoid
    phi    = geodetic latitude
    */

    double a2 = pow(a, 2);
    double h  = altitudeMSL;
    double h2 = pow(h, 2);

    double gammaH = gamma * (1 - (2/a)*(1 + f + m - 2*f*sin2phi)*h + (3/a2)*h2);

    gravity = gammaH;

}