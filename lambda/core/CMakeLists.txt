add_library( lambda_core
    ast.hpp
    ast.cpp
    parse_context.hpp
    parse_error.hpp
    parse.cpp
    parse.hpp
    token.cpp
    token.hpp
)

target_include_directories( lambda_core PUBLIC ${CMAKE_SOURCE_DIR} )
target_link_libraries( lambda_core PUBLIC ${CMAKE_DL_LIBS} -v
    cpptools::cpptools_static
)
