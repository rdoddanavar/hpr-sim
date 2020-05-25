# pybind 11 modules

set(SRC preproc/input.cpp)
set(HDR preproc/input.h)

pybind11_add_module(input MODULE ${SRC} ${HDR})