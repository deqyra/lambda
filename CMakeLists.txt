cmake_minimum_required( VERSION 3.24 )

set( CMAKE_CXX_STANDARD 23 )

# Project general info
project(
    lambda
    VERSION "1.0"
    DESCRIPTION "A lambda calculus engine"
    LANGUAGES C CXX
)

# Build stuff
add_subdirectory( lambda )
