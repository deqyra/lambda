#ifndef LAMBDA_SHELL_COMMANDS_EXPORT_COMMAND_HPP
#define LAMBDA_SHELL_COMMANDS_EXPORT_COMMAND_HPP

#include <lambda/shell/forceable_command.hpp>

namespace ld::shell {

class ExportCommand : public ForceableCommand {
public:
    ExportCommand()
        : ForceableCommand("export")
    {

    }

    code run(std::string_view command, bool force, ShellEnv& env) override {

        return code::success;
    }
};

} // namespace ld

#endif//LAMBDA_SHELL_COMMANDS_EXPORT_COMMAND_HPP