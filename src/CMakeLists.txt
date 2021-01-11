# pybind 11 modules

set(SRC preproc/input.cpp)
set(HDR preproc/input.h)

pybind11_add_module(input MODULE ${SRC} ${HDR})

# GNU GSL test

set(SRC model/test_gsl.cpp)
set(HDR model/test_gsl.h)

pybind11_add_module(test_gsl MODULE ${SRC} ${HDR})

target_link_libraries(test_gsl PUBLIC gsl)

# Expose all modules to library headers
include_directories("${CMAKE_SOURCE_DIR}/lib")