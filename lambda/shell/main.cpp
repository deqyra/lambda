#include <lambda/shell/shell_env.hpp>
#include <lambda/shell/shell.hpp>

#ifdef _WIN32
#   include <Windows.h>
#endif

using namespace ld;

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(65001); // UTF-8
#endif

    shell::ShellEnv env;
    shell::Shell shell;

    shell.run(env);

    return 0;
}