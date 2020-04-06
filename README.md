# hpr-sim

![](https://img.shields.io/badge/platform-linux--64-lightgrey.svg)
![](https://img.shields.io/github/license/rdoddanavar/hpr-sim.svg)
![](https://img.shields.io/github/repo-size/rdoddanavar/hpr-sim.svg)

## Overview

The goal of this project is to build a robust, extensible flight simulation package for high power rocketry, with open-source dependencies. Key features considered to constitute a "robust" simulation include:

 - 6-DOF kinematic model
 - Monte Carlo functionality
 - Multiprocessing to support large test cases 
 - Flexible atmospheric modeling
 - Stochastic wind turbulence model
 - Comptability w/ RASP (`*.eng`) engine files (http://www.thrustcurve.org/raspformat.shtml)
 - Flight data visualization & plain-text export
 - Simple input API for additional scripting & extensions

The intent is to lean on the respective strengths of python and C++ to create a modern, efficient simulation package. Python is used for pre-processing, post-processing, and data visualization, while C++ will constitute the bulk of the numerical routines. Pybind11 is used as a binding library to interface between the two sets of source code. 

Future work may include a user interface using PyQt5. Current efforts will be focused on a build for Ubuntu-based systems (x64). 

See `./doc/MANUAL.md` for program installation \& execution.

## Directory Index

```
.
├── doc/                   --> Supporting documentation
│   ├── figs/
│   ├── DEV.md
│   ├── FEATURES.md
│   ├── HEADER.md
│   ├── INPUT.md
│   └── MANUAL.md
├── input/                 --> Simulation input & config 
│   ├── aero/
│   ├── prop/
│   └── input.yaml
├── output/                --> Simulation data output
│   └── placeholder
├── src/                   --> Python & C++ source
│   ├── exec/
│   ├── models/
│   ├── postproc/
│   ├── preproc/
│   ├── solvers/
│   └── util/
├── tools/                 --> Project utilities 
│   ├── test/
│   └── update_index.sh*
├── LICENSE
├── makefile
└── README.md
```
