#ifndef LAMBDA_SHELL_COMMANDS_COMMON_HPP
#define LAMBDA_SHELL_COMMANDS_COMMON_HPP

#include <cpptools/cli/shell.hpp>

#include <lambda/shell/shell_env.hpp>

namespace ld::shell {

using shell_t = tools::cli::shell<ShellEnv>;

} // namespace ld::shell_cmd

#endif//LAMBDA_SHELL_COMMANDS_COMMON_HPP