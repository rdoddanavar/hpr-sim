# "model" module

set(SRC "./model/model_bind.cpp"
        "./model/test.cpp"
        "./model/engine.cpp"
        "./model/mass.cpp"
        "./model/geodetic.cpp"
        "./model/atmosphere.cpp"
        "./model/aerodynamics.cpp"
        "./model/eom.cpp"
        "./model/flight.cpp"
        "./exec/telem.cpp"
        )

set(HDR "./model/model.h"
        "./exec/telem.h")

set(DEP util_model
        gsl
        fmt
        )

pybind11_add_module(model MODULE ${SRC} ${HDR})
target_link_libraries(model PUBLIC ${DEP})

#------------------------------------------------------------------------------#

# "util_model" shared library

set(SRC "./util/util_model_interp.cpp")

set(HDR "./util/util_model.h")

set(DEP gsl)

add_library(util_model SHARED ${SRC} ${HDR})
target_link_libraries(util_model PUBLIC ${DEP})

#------------------------------------------------------------------------------#

# Expose all modules to library headers
include_directories("${CMAKE_SOURCE_DIR}/lib"
                    "./exec/"
                    "./model/"
                    "./util/"
                    )
