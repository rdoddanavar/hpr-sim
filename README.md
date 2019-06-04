# hpr-sim ![](https://img.shields.io/static/v1.svg?label=status&message=initial%20dev&color=red&style=for-the-badge)

`High Power Rocketry - Flight Simulation`

![](https://img.shields.io/badge/style-3.7+-blue.svg?label=python&logo=Python)
![](https://img.shields.io/badge/style-11-blue.svg?label=C%2B%2B&logo=C%2B%2B)
![](https://img.shields.io/static/v1.svg?label=pybind11&message=2.2.4&color=blue)

![](https://img.shields.io/badge/platform-linux--64-lightgrey.svg)
![](https://img.shields.io/github/license/rdoddanavar/hpr-sim.svg)
![](https://img.shields.io/github/repo-size/rdoddanavar/hpr-sim.svg)

## Overview

The goal of this project is to build a robust, extensible flight simulation package for high power rocketry, with open-source dependencies. Key features considered to constitute a "robust" simulation include:

 - 6-DOF kinematic model
 - Built-in Monte Carlo functionality
 - Multithreading to support " " 
 - Standard atmospheric modeling
 - Stochastic wind turbulence model
 - Comptability w/ RASP (`*.eng`) engine files (http://www.thrustcurve.org/raspformat.shtml)
 - Flight data visualization & plain-text export

The intent is to lean on the respective strengths of python and C++ to build a modern, efficient simulation package. Python is to be used for pre-processing, post-processing, and data visualization, while C++ will constitue the bulk of the numerical routines. A python-C++ binding library will be used to interface between the two sets of source code. 

Future work may include a user interface using PyQt5.

Other (more professional) projects with a similar scope & application are OpenRocket and RASAero II. 

See `./doc/MANUAL.md` for program installation \& execution.

![](./doc/figs/diagram.png)

## Directory Index

- `./bin/` - All binary objects
- `./doc/` - Supporting documentation
- `./input/` - Simulation input
- `./output/` - Simulation output
- `./src/` - Python & C++ source
- `./tools/` - Project utilities


```
.
├── bin/
│   └── input.cpython-37m-x86_64-linux-gnu.so*
├── doc/
│   ├── figs/
│   ├── DEV.md
│   ├── FEATURES.md
│   ├── HEADER.md
│   ├── INPUT.md
│   └── MANUAL.md
├── input/
│   ├── aero/
│   ├── prop/
│   └── input.yaml
├── output/
│   └── placeholder
├── src/
│   ├── exec/
│   ├── models/
│   ├── postproc/
│   ├── preproc/
│   ├── solvers/
│   └── util/
├── tools/
│   ├── test/
│   └── update_index.sh*
├── LICENSE
├── makefile
└── README.md

16 directories, 12 files
```
