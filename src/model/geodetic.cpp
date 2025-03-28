// System headers
#include <cmath>

// External headers
// <none>

// Internal headers
#include "model.h"

//---------------------------------------------------------------------------//

// WGS 84 Constants
const double gammaE = 9.7803253359;        // [m/s^2] Normal gravity at the equator (on the ellispoid)
const double k      = 1.931852652458e-03;  // [-]     Somigliana’s Formula - normal gravity formula constant
const double e      = 8.1819190842622e-02; // [-]     First eccentricity of the ellispoid
const double a      = 6378137.0;           // [m]     Semi-major axis of the ellipsoid
const double b      = 6356752.3142;        // [m]     Semi-minor axis of the ellipsoid
const double f      = 3.3528106647475e-03; // [-]     WGS 84 flattening (reduced)
const double m      = 3.449786506841e-03;  // [-]     Normal gravity formula constant (w^2*a^2*b/GM)

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

    isInit_ = true;

}

//---------------------------------------------------------------------------//

void Geodetic::set_state_fields()
{

    state->emplace("altitudeMSL", &altitudeMSL);
    state->emplace("altitudeAGL", &altitudeAGL);
    state->emplace("altitudeGP", &altitudeGP);
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

    gamma  = Normal gravity at surface of ellipsoid
    phi    = Geodetic latitude
    */

   // TODO: calculate latitude drift with rocket drift downrange?
   // This would require the subroutine to be folded back into wgs84()

           a2      = pow(a, 2);
    double a4      = pow(a, 4);
    double b2      = pow(b, 2);
    double b4      = pow(b, 4);
    double e2      = pow(e, 2);
           sin2phi = pow(sin(phi), 2);
    double cos2phi = pow(cos(phi), 2);

    gamma = gammaE * (1 + k*sin2phi) / sqrt(1 - e2*sin2phi);

    // Geocentric radius (for geopotential)
    radiusE = sqrt((a4*cos2phi + b4*sin2phi) / (a2*cos2phi + b2*sin2phi));

}

//---------------------------------------------------------------------------//

void Geodetic::wgs84()
{

    /*
    NGA.STND.0036_1.0.0_WGS84
    Normal Gravity Above the Ellipsoid - Taylor series expansion (4-3)

    h      = Height above ellispoid
    gammaH = Normal gravity at height above ellipsoid
    */

    double h  = altitudeMSL;
    double h2 = pow(h, 2);

    double gammaH = gamma * (1 - (2/a)*(1 + f + m - 2*f*sin2phi)*h + (3/a2)*h2);

    gravity = gammaH;

    // Geopotential altitude
    altitudeGP = (radiusE*h) / (radiusE + h);

}
