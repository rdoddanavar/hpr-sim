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

The intent is to lean on the respective strengths of python and C++ to create a modern, efficient simulation package. Python is used for pre-processing, post-processing, and data visualization, while C++ constitutes the bulk of the numerical routines. Pybind11 is used as a binding library to interface between the two sets of source code. 

Current efforts are focused on a build for Ubuntu-based linux systems (x64). 

## Dependencies

See `doc/MANUAL.md` for project build, execution, & dependency handling. 

| Package  | Version  | Purpose                    | Link                                         |
|----------|----------|----------------------------|----------------------------------------------|
| g++      | `x.x.x`  | C++ compiler               |                                              |
| eigen    | `3.3.7`  | Linear algebra library     | https://gitlab.com/libeigen/eigen/-/releases |
| Anaconda | `3.7`    | Python distribution        | https://www.anaconda.com/distribution/       |
| pybind11 | `2.5.0`  | Python/C++ binding library | https://github.com/pybind/pybind11           |
| CMake    | `3.10.2` | Build system manager       | https://cmake.org/install/                   |

## Directory Index

 - `build/` - CMake build directory
 - `doc/` - Manual & supporting documentation
 - `input/` - Simulation & model input
 - `lib/` - External libraries
 - `output/` - Simulation data output
 - `src/` - Python & C++ source
 - `tools/` - Misc. utilities & test scripts 

```
.
├── build
├── doc
│   ├── figs
│   ├── DEV.md
│   ├── FEATURES.md
│   ├── HEADER.md
│   ├── INPUT.md
│   └── MANUAL.md
├── input
│   ├── aero
│   ├── prop
│   └── input.yaml
├── lib
│   └── eigen-3.3.7.tar.gz
├── output
│   └── placeholder
├── src
│   ├── exec
│   ├── models
│   ├── postproc
│   ├── preproc
│   ├── solvers
│   ├── util
│   └── CMakeLists.txt
├── tools
│   ├── gui
│   ├── test
│   ├── makefile_old
│   └── update_index.sh
├── CMakeLists.txt
├── LICENSE
└── README.md
```
