# hpr-sim

## Overview

The goal of this project is to build a robust, extensible flight simulation package for high power rocketry, with open-source dependencies. Key features considered to constitute a "robust" simulation include:

 - 6-DOF kinematic model
 - Built-in Monte Carlo functionality
 - Standard atmospheric modeling
 - Stochastic wind turbulence model
 - Comptability w/ RASP (`*.eng`) engine files (http://www.thrustcurve.org/raspformat.shtml)

The intent is to lean on the respective strengths of python and C++ to build a modern, efficient simulation package. Python is to be used for pre-processing, post-processing, and data visualization, while C++ will constitue the bulk of the numerical routines. A python-C++ binding library will be used to interface between the two sets of source code. 

Future work may include a user interface using PyQt5.

Other projects with a similar scope & application are Rocksim, OpenRocket, and RASAero II. 

## Project Index

`tree -L 1 --dirsfirst`

```
.                                                                                         
├── bin                                                                                   
├── build                                                                                 
├── input                                                                                 
├── output                                                                                
├── src                                                                                   
├── MANUAL.md                                                                             
└── README.md                                                                             
                                                                                          
5 directories, 2 files   
```
- `./bin/` - All binary object(s)
- `./build/` - Makefile & build config
- `./input/` - Input file(s)
- `./output/` - Simulation output
- `./src/` - Python & C++ source
- `./MANUAL.md` - Program manual
