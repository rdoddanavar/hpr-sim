# Algorithms

## Geodetic Model

To determine the local gravitational acceleration, the WGS84 geodetic model is used, from the report `NGA.STND.0036_1.0.0_WGS84` (2014-07-08) [^myref].

1. Using $\boxed{\phi}$, find normal gravity on the ellipsoidal surface:

$$
    \gamma = \gamma_e \frac{1 + k \sin^2 \phi}{\sqrt{1 - e^2 \sin^2 \phi}}
$$

2. Then, use $\boxed{\gamma, h}$ to find normal gravity above the ellipsoid:

$$
    \gamma_h = \gamma \bigg[ 1 - \frac{2}{a} \left( 1 + f + m - 2f \sin^2 \phi \right) h + \frac{3}{a^2} h^2 \bigg]
$$

## Atmospheric Model

## References

[^myref]: (citation)