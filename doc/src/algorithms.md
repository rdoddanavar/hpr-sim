# Algorithms

## Geodetic Model

To determine the local gravitational acceleration, the WGS84 geodetic model is used from `NGA.STND.0036_1.0.0_WGS84` (2014-07-08) [^wgs84] :

1. Using launch site latitude $\boxed{\phi}$, the normal gravity $\boxed{\gamma}$ is found on the ellipsoidal surface (Somigliana's formula):

$$
    \gamma = \gamma_e \frac{1 + k \sin^2 \phi}{\sqrt{1 - e^2 \sin^2 \phi}}
    \qquad [m/s^2]
$$

<center>

| Constant   | Value              |
|------------|--------------------|
| $\gamma_e$ | 9.7803253359       |
| $k$        | 1.931852652458e-3  |
| $e$        | 8.1819190842622e-2 |

</center>

2. Then, $\boxed{\gamma}$ is used to find normal gravity $\boxed{\gamma_h}$ at a height $\boxed{h}$ above the ellipsoid:

$$
    \gamma_h = \gamma \bigg[ 1 - \frac{2}{a} \left( 1 + f + m - 2f \sin^2 \phi \right) h + \frac{3}{a^2} h^2 \bigg] \quad [m/s^2]
$$

<center>

| Constant | Value             |
|----------|-------------------|
| $a$      | 6378137.0         |
| $1/f$    | 298.257223563     |
| $m$      | 3.449786506841e-3 |

</center>

## Atmospheric Model

US Standard Atmosphere 1976 [^us1976]

## References

[^wgs84]: https://earth-info.nga.mil/php/download.php?file=coord-wgs84
[^us1976]: https://www.ngdc.noaa.gov/stp/space-weather/online-publications/miscellaneous/us-standard-atmosphere-1976/us-standard-atmosphere_st76-1562_noaa.pdf