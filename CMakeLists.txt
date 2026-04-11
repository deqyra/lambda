cmake_minimum_required( VERSION 3.24 )

set( CMAKE_CXX_STANDARD 23 )

# Project general info
project(
    lambda
    VERSION "1.0"
    DESCRIPTION "A lambda calculus engine"
    LANGUAGES C CXX
)

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
