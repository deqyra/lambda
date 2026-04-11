#ifndef LAMBDA_SHELL_COMMANDS_IMPORT_COMMAND_HPP
#define LAMBDA_SHELL_COMMANDS_IMPORT_COMMAND_HPP

#include <lambda/shell/forceable_command.hpp>

namespace ld::shell {

class ImportCommand : public ForceableCommand {
public:
    ImportCommand()
        : ForceableCommand("import")
    {

    }

    code run(std::string_view command, bool force, ShellEnv& env) override {

        return code::success;
    }
};

} // namespace ld

#endif//LAMBDA_SHELL_COMMANDS_IMPORT_COMMAND_HPP