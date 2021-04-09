# INPUT

set(SRC "./preproc/input.cpp")
set(HDR "./preproc/input.h")

pybind11_add_module(input MODULE ${SRC} ${HDR})

#-----------------------------------------------------------------------------#

# "model" module

set(SRC "./model/model.cpp"
        "./model/engine.cpp"
        "./model/geodetic.cpp"
        "./model/eom.cpp"
        "./model/flight.cpp"
        )

set(HDR "./model/model.h")

set(DEP util
        gsl
        )

pybind11_add_module(model MODULE ${SRC} ${HDR})
target_link_libraries(model PUBLIC ${DEP})

#-----------------------------------------------------------------------------#

# "util" shared library

set(SRC "./util/util_interp.cpp")

set(HDR "./util/util_interp.h"
        "./util/util_ode.h"
        )

set(DEP gsl)

add_library(util SHARED ${SRC} ${HDR})
target_link_libraries(util PUBLIC ${DEP})

#-----------------------------------------------------------------------------#

# Expose all modules to library headers
include_directories("${CMAKE_SOURCE_DIR}/lib"
                    "./model/"
                    "./util/"
                    )