add_executable( lambda_shell
    common.hpp
    main.cpp
    shell_env.hpp
    shell.hpp
)

target_include_directories( lambda_shell PUBLIC ${CMAKE_SOURCE_DIR} )
target_link_libraries( lambda_shell PUBLIC ${CMAKE_DL_LIBS} -v lambda_core )
