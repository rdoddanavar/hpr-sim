# Algorithms

## Geodetic Model

To determine the local gravitational acceleration, the WGS84 geodetic model is used, from the report `NGA.STND.0036_1.0.0_WGS84` (2014-07-08) [^wgs84] :

1. Using launch site latitude $\boxed{\phi}$, find normal gravity $\boxed{\gamma}$ on the ellipsoidal surface (Somigliana's formula):

$$
    \gamma = \gamma_e \frac{1 + k \sin^2 \phi}{\sqrt{1 - e^2 \sin^2 \phi}}
    \qquad [m/s^2]
$$

$$
    \text{where:} \qquad
    \begin{array}{ll}
        \gamma_e & 9.7803253359                  \\
        k        & 1.931852652458 \times 10^{-3} \\
        e        & 6.694379990141 \times 10^{-3}
    \end{array}
$$

2. Then, use $\boxed{\gamma}$ to find normal gravity $\boxed{\gamma_h}$ at a height $\boxed{h}$ above the ellipsoid:

$$
    \gamma_h = \gamma \bigg[ 1 - \frac{2}{a} \left( 1 + f + m - 2f \sin^2 \phi \right) h + \frac{3}{a^2} h^2 \bigg] \quad [m/s^2]
$$

$$
    \text{where:} \qquad
    \begin{array}{ll}
        a   & \\
        a^2 & \\
        f   & \\
        m   &
    \end{array}
$$

## Atmospheric Model

US Standard Atmosphere 1976 [^us1976]

## References

[^wgs84]: https://earth-info.nga.mil/php/download.php?file=coord-wgs84
[^us1976]: https://www.ngdc.noaa.gov/stp/space-weather/online-publications/miscellaneous/us-standard-atmosphere-1976/us-standard-atmosphere_st76-1562_noaa.pdf