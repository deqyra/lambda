cmake_minimum_required( VERSION 3.24 )

set( CMAKE_CXX_STANDARD 23 )

# Project general info
project(
    lambda
    VERSION "1.0"
    DESCRIPTION "A lambda calculus engine"
    LANGUAGES C CXX
)

option( LAMBDA_BUILD_TESTS "Whether or not to build and run Lambda unit tests" OFF )

include( FetchContent )

# cpptools
FetchContent_Declare(
    cpptools
    DOWNLOAD_EXTRACT_TIMESTAMP ON
    URL https://github.com/deqyra/CppTools/archive/refs/tags/v1.1.zip
    FIND_PACKAGE_ARGS 1.1
)

set( CPPTOOLS_SKIP_TESTS ON )
FetchContent_MakeAvailable( cpptools )

# Build stuff
add_subdirectory( lambda )

# Build and run tests
if( LAMBDA_BUILD_TESTS )
    FetchContent_Declare(
        Catch2
        DOWNLOAD_EXTRACT_TIMESTAMP ON
        URL https://github.com/catchorg/Catch2/archive/refs/tags/v3.5.1.zip
        FIND_PACKAGE_ARGS
    )

    FetchContent_MakeAvailable( Catch2 )
    list( APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras )

    include( CTest )
    include( Catch )

    add_subdirectory( tests )

    add_test(
        NAME run_lambda_tests 
        COMMAND lambda_tests
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endif(  )