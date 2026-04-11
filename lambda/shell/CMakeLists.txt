add_executable( lambda_shell
    common.hpp
    forceable_command.hpp
    main.cpp
    shell_env.hpp
    shell.hpp
    commands/declare_command.hpp
    commands/export_command.hpp
    commands/help_command.hpp
    commands/import_command.hpp
    commands/reduce_command.hpp
)

target_include_directories( lambda_shell PUBLIC ${CMAKE_SOURCE_DIR} )
target_link_libraries( lambda_shell PUBLIC ${CMAKE_DL_LIBS} -v cpptools::cpptools_static lambda_core )
