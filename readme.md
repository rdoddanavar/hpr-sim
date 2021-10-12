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

## System Dependencies

Must be present on user's machine to build & run simulation:

|          | Version   | Purpose              | Link                                   |
|----------|-----------|--------------------- |--------------------------------------- |
| gcc      | `4.8+`    | C/C++ compiler       |                                        |
| Anaconda | `3.7+`    | Python distribution  | https://www.anaconda.com/distribution/ |
| CMake    | `3.10.2+` | Build system manager | https://cmake.org/install/             |

## External Libraries

Managed through `git submodule`; included with repository by default:

|          | Version | Purpose                       | Link                               |
|----------|---------|-------------------------------|----------------------------------- |
| eigen    | `3.4.0` | Linear algebra utilities      | https://gitlab.com/libeigen/eigen  |
| gsl      | `2.7.0` | Scientific/numerical utilties | https://github.com/ampl/gsl        |
| pybind11 | `2.7.1` | Python/C++ bindings           | https://github.com/pybind/pybind11 |

## Directory Index

 - `build/` - CMake build directory
 - `config/` - Default simulation settings
 - `doc/` - Manual & supporting documentation
 - `input/` - Simulation & model input
 - `lib/` - External libraries
 - `output/` - Simulation data output
 - `src/` - Python & C++ source
 - `tools/` - Misc. utilities & test scripts 

```
.
├── build/
├── config/
├── doc/
├── input/
│   ├── aero/
│   ├── engine/
│   └── unit_test.yml
├── lib/
│   ├── eigen/
│   ├── gsl/
│   └── pybind11/
├── output/
├── src/
│   ├── exec/
│   ├── model/
│   ├── postproc/
│   ├── preproc/
│   ├── util/
│   └── CMakeLists.txt
├── tools/
├── CMakeLists.txt
├── hpr-sim.py*
├── license
└── readme.md

21 directories, 17 files
```
