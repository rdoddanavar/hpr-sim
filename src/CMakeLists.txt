# input/

set(SRC preproc/input.cpp)
set(HDR preproc/input.h)

pybind11_add_module(input MODULE ${SRC} ${HDR})

#-----------------------------------------------------------------------------#

## GNU GSL TEST
#
#set(SRC model/test_gsl.cpp)
#set(HDR model/test_gsl.h)
#
#pybind11_add_module(test_gsl MODULE ${SRC} ${HDR})
#
#target_link_libraries(test_gsl PUBLIC gsl)

#-----------------------------------------------------------------------------#

# model/

set(SRC "./model/engine.cpp"
        )

set(HDR "./model/model.h"
        "./model/engine.h"
        )

set(DEP "util"
        "gsl"
        )

pybind11_add_module(model MODULE ${SRC} ${HDR})
target_link_libraries(model PUBLIC ${DEP})

#-----------------------------------------------------------------------------#

# util/

set(SRC "./util/util_interp.cpp")
set(HDR "./util/util_interp.h")
set(DEP "gsl")

add_library(util SHARED ${SRC} ${HDR})
target_link_libraries(util PUBLIC ${DEP})

#-----------------------------------------------------------------------------#

# Expose all modules to library headers
include_directories("${CMAKE_SOURCE_DIR}/lib"
                    "./model/"
                    "./util/")