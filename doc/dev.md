# hpr-sim

`High Power Rocketry - Flight Simulation`

## Reading Backlog

 - `eval` alternatives
   - https://stackoverflow.com/questions/43836866/safely-evaluate-simple-string-equation
 - Recursion for `dict`'s
   - https://stackoverflow.com/questions/22162321/search-for-a-value-in-a-nested-dictionary-python

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
 - Pathlib example
   - https://stackoverflow.com/questions/40416072/reading-file-using-relative-path-in-python-project/40416154
 - XML
   - https://stackabuse.com/reading-and-writing-xml-files-in-python/

### Pyinstaller 

`LOL`

## C++

### General 

 - Order of operations
   - https://en.cppreference.com/w/cpp/language/operator_precedence
 - Reserved keywords
   - https://en.cppreference.com/w/cpp/keyword
 - Check for file existence
   - https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
 - Convert `std::vector` to array
   - https://stackoverflow.com/questions/2923272/how-to-convert-vector-to-array
 - Create deliminated string from vector
   - https://stackoverflow.com/questions/20817322/convert-vectorint-to-delimited-string
   - https://stackoverflow.com/questions/9277906/stdvector-to-string-with-custom-delimiter
 - Writing formatted string to `iostream` or `fstream`
   - https://stackoverflow.com/questions/11989374/floating-point-format-for-stdostream

### Object-Oriented

 - `virtual` keyword: https://stackoverflow.com/questions/2391679/why-do-we-need-virtual-functions-in-c?rq=1
 - `override` keyword: https://stackoverflow.com/questions/18198314/what-is-the-override-keyword-in-c-used-for
 - pointers to `std::map`: https://stackoverflow.com/questions/7482888/pointer-to-a-map

### Template Classes
 - http://www.cplusplus.com/doc/oldtutorial/templates/
   - Declaration & implementation in same file
 - Default constructors
   - https://stackoverflow.com/questions/15604277/default-constructor-in-template-class-with-attribute-of-unknown-type

### Pybind11

 - Example binding

```
PYBIND11_MODULE(module, m)
{
    m.doc() = "docstring"; // Optional module docstring

    // Function declaration
    m.def("add", &add, "A function which adds two numbers");

    // Class declaration
    py::class_<Class>(m, "Class")
        .def(py::init<>()) // Specify type as template parameter
        .def("method", &Class::method)
        .def_readwrite("attribute", &Class::attribute)
}
```

 - Split binding over multiple files: https://stackoverflow.com/questions/53762552/with-pybind11-how-to-split-my-code-into-multiple-modules-files
 - BE CAREFUL!!! When splitting bindings, keep code in `*.cpp`, not header `*.h`; if you include binding in multiple places it will violate "One Definition Rule"; see issue #2157

```
Release Notes (pybind11 v2.6.0)

py::module was renamed py::module_ to avoid issues with C++20 when used unqualified, but an alias
py::module is provided for backward compatibility. #2489
```

 - Template class binding (unresolved)
   - https://github.com/pybind/pybind11/issues/199
   - https://stackoverflow.com/questions/47487888/pybind11-template-class-of-many-types

 - pybind11 working with numpy arrays: https://stackoverflow.com/questions/58756764/return-an-stdvector-to-python-as-a-numpy-array

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

## CMake

 - List build targets
   - https://stackoverflow.com/questions/30793804/how-do-i-list-the-defined-make-targets-from-the-command-line 
 - Group dependencies
   - https://stackoverflow.com/questions/37189937/group-build-targets-in-cmake-project


## Errors & Logging