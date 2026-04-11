add_library( lambda_core INTERFACE
    parse_error.hpp
    token.hpp
)

target_include_directories( lambda_core INTERFACE ${CMAKE_SOURCE_DIR} )
target_link_libraries( lambda_core INTERFACE ${CMAKE_DL_LIBS} -v cpptools::cpptools_static )
