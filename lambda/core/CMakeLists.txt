add_library( lambda_core INTERFACE
    parse_error.hpp
)

target_include_directories( lambda_core INTERFACE ${CMAKE_SOURCE_DIR} )
