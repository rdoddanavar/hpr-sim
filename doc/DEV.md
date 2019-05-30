# hpr-sim

`High Power Rocketry - Flight Simulation`

## Reading Backlog

 - `eval` alternatives
   - https://stackoverflow.com/questions/43836866/safely-evaluate-simple-string-equation

## Python

### Style

 - PEP-8 style guide
   - https://www.python.org/dev/peps/pep-0008/
 - PEP-257 docstrings
   - https://www.python.org/dev/peps/pep-0257/

### General

 - `if __name__ == "__main__"` stuff
   - https://stackoverflow.com/questions/419163/what-does-if-name-main-do
 - Program timing (`tic` + `toc` alternative)
   - https://stackoverflow.com/questions/7370801/measure-time-elapsed-in-python
 - Mutiple function arguments
   - https://stackoverflow.com/questions/919680/can-a-variable-number-of-arguments-be-passed-to-a-function
 - `__pycache__` bullshit
   - https://stackoverflow.com/questions/16869024/what-is-pycache
 - Errors & Logging
   - https://docs.python.org/3/tutorial/errors.html
   - https://realpython.com/python-logging/


## C++

### General 

 - Order of operations
   - https://en.cppreference.com/w/cpp/language/operator_precedence
 - Reserved keywords
   - https://en.cppreference.com/w/cpp/keyword
 - Check for file existence
   - https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c

### Template Classes
 - http://www.cplusplus.com/doc/oldtutorial/templates/
   - Declaration & implementation in same file
 - Default constructors
   - https://stackoverflow.com/questions/15604277/default-constructor-in-template-class-with-attribute-of-unknown-type

### OpenMP

### Pybind11

 - Example binding

```
PYBIND11_MODULE(module, m)
{
    m.doc() = "docstring"; // Optional module docstring

    py::class_<Class>(m, "Class")
        .def(py::init<>()) // Specify type as template parameter
        .def("method", &Class::method)
        .def_readwrite("attribute", &Class::attribute)
}
```

 - Template class binding (unresolved)
   - https://github.com/pybind/pybind11/issues/199
   - https://stackoverflow.com/questions/47487888/pybind11-template-class-of-many-types

## YAML

### References

 - https://yaml.org/refcard.html
 - https://en.wikipedia.org/wiki/YAML
 - Online parser
   - https://yaml-online-parser.appspot.com/


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

## Documentation

 - Sphinx - "Read the Docs" - "reST" - restructured text
   - https://docs.python-guide.org/writing/documentation/
 - Docstring conventions
   - https://stackoverflow.com/questions/3898572/what-is-the-standard-python-docstring-format
 - Python default: Sphinx, C++ default: doxygen
   - interface library: breathe? and exhale? 

## Testing