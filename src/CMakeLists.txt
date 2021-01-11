# pybind 11 modules

set(SRC preproc/input.cpp)
set(HDR preproc/input.h)

pybind11_add_module(input MODULE ${SRC} ${HDR})

# GNU GSL test

set(SRC model/test_gsl.cpp)
set(HDR model/test_gsl.h)

pybind11_add_module(test_gsl MODULE ${SRC} ${HDR})

target_include_directories(test_gsl PUBLIC "${CMAKE_SOURCE_DIR}/build/lib/gsl")