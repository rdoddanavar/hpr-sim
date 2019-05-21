# hpr-sim

`High Power Rocketry - Flight Simulation`

## C++

### General 

 - Check for file existence
   - https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c

### Template Classes
 - http://www.cplusplus.com/doc/oldtutorial/templates/
   - Declaration & implmentation in same file
 - Default constructors
   - https://stackoverflow.com/questions/15604277/default-constructor-in-template-class-with-attribute-of-unknown-type

### OpenMP

### Pybind11
 - Template class binding
   - https://github.com/pybind/pybind11/issues/199
   - https://stackoverflow.com/questions/47487888/pybind11-template-class-of-many-types

## Python

## YAML

### Specification
 - Infinity --> `.inf`
 - Scientific notation must have period in mantissa and sign on exponent
   - ex. `1.0e+5`
 - Empty array --> `field: []`

### pyYAML
 - Get stream and pass to parser
   - `stream = open("filename",'r')` --> `dict = yaml.load(stream)`
 - Call obj using dict format, double up for levels
   - `value = dict["lvl1"]["lvl2"]` etc.