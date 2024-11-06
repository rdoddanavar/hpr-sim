# hpr-sim

![](https://img.shields.io/github/license/rdoddanavar/hpr-sim.svg)
![](https://img.shields.io/github/repo-size/rdoddanavar/hpr-sim.svg)

![](doc/logo.svg)

- [Overview](#overview)
- [Usage](#usage)
- [Benchmarking](#benchmarking)
- [External Libraries](#external-libraries)
- [Directory Index](#directory-index)
- [FAQ](#faq)

## Overview

> **NOTE: This project is still in development and not yet in a release state**

The goal of this project is to build a robust, extensible flight simulation package for high power rocketry, with open-source dependencies.

Key features:

 - 6-DOF kinematic model
 - Monte Carlo functionality
 - Parallel processing (multicore) to support large test cases 
 - High fidelity environment modeling: atmospheric, geodetic, wind turbulence, etc.
 - Comptability w/ RASP (`*.eng`) engine files (http://www.thrustcurve.org/raspformat.shtml)
 - Flight data visualization & plain-text export
 - Simple input API for additional scripting & extensions

Current efforts are focused on builds for these platforms:

| Platform            | Runner OS      | Test(s)                                                                                     
| --------------------|----------------|---------------------------------------------------------------------------------------------
| Linux (x86_64)      | `ubuntu-22.04` | ![](https://github.com/rdoddanavar/hpr-sim/actions/workflows/unit-test-ubuntu.yml/badge.svg)
| Windows 10 (x86_64) | `windows-2022` | ![](https://github.com/rdoddanavar/hpr-sim/actions/workflows/unit-test-windows.yml/badge.svg)
| Mac (arm64)         | n/a            | To be supported in the future

## Usage

To build the simulation:

```bash
$ cd build
$ cmake --preset ubuntu ../ # Configs available for "ubuntu" or "windows"
$ cmake --build . -j # Use all available cores
```

To run the simulation in CLI mode (headless):

```bash
$ ./hpr-sim.py -i input/unit_test.yml -o output
```

To launch the GUI, simply call the program with no arguments:

```bash
$ ./hpy-sim.py
```

CLI help:

```bash
$ ./hpr-sim.py -h # Trigger help message

usage: hpr-sim.py [-h] [-i INPUT] [-o OUTPUT]

options:
  -h, --help            show this help message and exit
  -i INPUT, --input INPUT
                        Input file; if none given, user interface will launch
  -o OUTPUT, --output OUTPUT
                        Output directory; if none given, assumed to be output/
```

## Benchmarking

The unit test used for benchmarking simulates the flight of a 4 in Patriot replica kit (example: ) with a XXX motor:

| Platform     | Build Type | Processors | Number of Sims | Run Time | Unit Run Time |
|--------------|------------|------------|----------------|----------|---------------|
| Ubuntu 22.04 | Debug      | 8          |          1,000 | XXX s    | XXX s         |
|              | Release    | 8          |          1,000 | XXX s    | XXX s         |
| Windows 10   | Debug      | 8          |          1,000 | XXX s    | XXX s         |
|              | Release    | 8          |          1,000 | XXX s    | XXX s         |

## External Libraries

Managed through `git submodule`; included with repository by default:

|          | Version  | Purpose                       | Link                               |
|----------|----------|-------------------------------|----------------------------------- |
| eigen    | `3.4.0`  | Linear algebra utilities      | https://gitlab.com/libeigen/eigen  |
| fmt      | `11.0.2` | String formatting & output    | https://github.com/fmtlib/fmt      |
| gsl      | `2.7.0`  | Scientific/numerical utilties | https://github.com/ampl/gsl        |
| pybind11 | `2.12.0` | Python/C++ bindings           | https://github.com/pybind/pybind11 |

## Directory Index

 - `build` - CMake build directory
   - `pyinstaller` - PyInstaller build output
 - `doc` - Manual & supporting documentation
 - `input` - Simulation & model input
 - `lib` - External C/C++ libraries (submodules)
 - `output` - Simulation data output
 - `src` - Python & C++ source
   - `exec` - Simulation control & execution
   - `gui` - Graphical user interface via PyQt
   - `model` - Rocket & environment models
   - `postproc` - Simulation post-processing: data reduction, plots, etc.
   - `preproc` - Simulation pre-processing: input validation & model setup
   - `test` - Model unit test scripts
   - `util` - General utilities
 - `tools` - Developer tools & scripts

## FAQ

### Why is there a mix of Python & C++ in this project?

- The intent is to lean on the respective strengths of python and C++:
    - Python is used for pre-processing, post-processing, and data visualization
    - C++ constitutes the bulk of the numerical routines
- pybind11 is used as a binding library to wrap the C++ routines and build them as (importable) python modules

### Why not use [OpenRocket](https://openrocket.info/) ?

- Use both! They are distinct tools with different features & use-cases:
  - OpenRocket can be a used as a first-step in designing or simulating a model rocket
    - CAD-like user interface with part picker and motor database
  - hpr-sim provides advanced simulation capabilities to dig deeper into a rocket's performance and gather analysis for launch safety & recovery
    - Monte Carlo capability, dispersion analysis, parameter optimization
