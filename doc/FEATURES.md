# hpr-sim

`High Power Rocketry - Flight Simulation`

## Proposed/Future Features

### Utilities

 - Regression set --> input files
 - Regression report
 - Debug compile mode

### Pre-processing

 - YAML relational anchors
 - PyYAML eval string arithmetic expressions 
 - PyQt5 GUI to gen input files 

### Atmospheric modeling

 - Distinct turbulence models
 - Expand reference frame
 - Lat/long input, find alt from model, provide manual correction

### Aero modeling

### Prop modeling
 - Rough estimate (for COTS solids):
   - mass flow rate
   - port diameter --> mom intertia's 
   - 

### Post-processing & viz

 - Plain text `*.dat` & `*.log` dumps
   - `*.dat`: comma delimited, variable and unit headers
   - `*.log`: sim stats & events
 - Pickle archive --> after processing raw dump
 - PyQt5 inspection GUI?
 - Simple LaTeX sim report