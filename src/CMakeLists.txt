set(SRC "./model/model_bind.cpp"
        "./model/test.cpp"
        "./model/engine.cpp"
        "./model/mass.cpp"
        "./model/geodetic.cpp"
        "./model/atmosphere.cpp"
        "./model/aerodynamics.cpp"
        "./model/eom.cpp"
        "./model/flight.cpp"
        "./model/telem.cpp"
        "./model/interp.cpp"
        )

pybind11_add_module(model MODULE ${SRC})
target_link_libraries(model PUBLIC gsl fmt)
target_include_directories(model PUBLIC "${CMAKE_SOURCE_DIR}/lib")
