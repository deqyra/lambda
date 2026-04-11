#ifndef LAMBDA_SHELL_SHELL_HPP
#define LAMBDA_SHELL_SHELL_HPP

#include <cpptools/cli/shell.hpp>

#include <lambda/shell/shell_env.hpp>
#include <lambda/shell/common.hpp>
namespace ld::shell {

class Shell {
    shell_t _shell;

    static constexpr std::string_view PreambleString =
R"(lambda v0 (c) François Brachais. Type '?' to get help.
Report bugs at https://github.com/deqyra/lambda or francois.brachais[at]free.fr

)";

public:

    void run(ShellEnv& env) {
        env.shell_streams().out << PreambleString;
        _shell.run(env);
    }
};

} // namespace ld

#endif//LAMBDA_SHELL_SHELL_HPP