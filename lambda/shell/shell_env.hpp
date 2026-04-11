#ifndef LAMBDA_SHELL_SHELL_ENV_HPP
#define LAMBDA_SHELL_SHELL_ENV_HPP

#include <iostream>
#include <map>

#include <cpptools/cli/shell.hpp>
#include <cpptools/utility/heterogenous_lookup.hpp>

#include <lambda/core/parse.hpp>

namespace ld::shell {

struct ShellEnv : public tools::cli::default_shell_environment {
    std::string_view shell_prompt_head() const {
        return "> ";
    }

    tools::cli::streams& shell_streams() {
        static auto s = tools::cli::streams{ std::cin, std::cout, std::cerr };
        return s;
    }

    std::map<std::string, AST, tools::any_transparent_less> declarations;
};
static_assert(tools::cli::shell_environment<ShellEnv>);

} // namespace ld

#endif//LAMBDA_SHELL_SHELL_ENV_HPP