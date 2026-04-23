################################################################################
#                                                                              #
#                                 Unit tests                                   #
#                                                                              #
################################################################################

set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} )

add_executable( lambda_tests
    ostream_dump.cpp
    to_string.cpp
    core/test_token.cpp
    core/test_parse.cpp
)
target_include_directories( lambda_tests PRIVATE
    ${CMAKE_SOURCE_DIR}
)
target_link_libraries( lambda_tests PUBLIC
    ${CMAKE_DL_LIBS}
    lambda_core
    Catch2::Catch2WithMain
) 

catch_discover_tests( lambda_tests
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
