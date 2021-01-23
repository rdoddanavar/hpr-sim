#pragma once

// System libraries

// Project headers
#include "gsl/interpolation/gsl_interp.h"
#include "gsl/interpolation/gsl_spline.h"

// Prototypes

void interp1d_init(const double x[], const double y[], size_t n, gsl_spline* &spline, gsl_interp_accel* &acc);

double interp1d_eval(gsl_spline* spline, double xq, gsl_interp_accel* acc);