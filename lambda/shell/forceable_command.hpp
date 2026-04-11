#ifndef LAMBDA_SHELL_FORCEABLE_COMMAND_HPP
#define LAMBDA_SHELL_FORCEABLE_COMMAND_HPP

#include <string_view>

#include <cpptools/utility/string.hpp>

#include <lambda/shell/common.hpp>

namespace ld::shell {

class ForceableCommand : public shell_t::command {
public:
    ForceableCommand(std::string_view prefix)
        : _prefix(prefix)
    {

    }

    virtual code run(std::string_view command, bool force, ShellEnv& env) = 0;

    code run(std::string_view command, ShellEnv& env) override {
        bool force = false;
        command.remove_prefix(_prefix.size());
        if (command.size() > 0 and *command.begin() == '!') {
            force = true;
            command.remove_prefix(1);
        }

        command = tools::trim(command);
        return run(command, force, env);
    }

private:
    std::string_view _prefix;
};

} // namespace ld

#endif//LAMBDA_SHELL_COMMANDS_DECLARE_COMMAND_HPP