#ifndef LAMBDA_SHELL_SHELL_HPP
#define LAMBDA_SHELL_SHELL_HPP

#include <cpptools/cli/shell.hpp>

#include <lambda/shell/shell_env.hpp>
#include <lambda/shell/common.hpp>
#include <lambda/shell/commands/export_command.hpp>
#include <lambda/shell/commands/declare_command.hpp>
#include <lambda/shell/commands/help_command.hpp>
#include <lambda/shell/commands/import_command.hpp>
#include <lambda/shell/commands/reduce_command.hpp>

namespace ld::shell {

class Shell {
    shell_t _shell;

    ExportCommand _export;
    DeclareCommand _declare;
    HelpCommand _help;
    ImportCommand _import;
    ReduceCommand _reduce;

    static constexpr std::string_view PreambleString =
R"(lambda v0 (c) François Brachais. Type '?' to get help.
Report bugs at https://github.com/deqyra/lambda or francois.brachais[at]free.fr

)";

public:
    Shell() {
        _shell.set_default_command(_reduce);
        _shell.set_command("export", _export);
        _shell.set_command("export!", _export);
        _shell.set_command("import", _import);
        _shell.set_command("import!", _import);
        _shell.set_command("let", _declare);
        _shell.set_command("let!", _declare);
        _shell.set_command("help", _help);
        _shell.set_command("?", _help);
        _shell.add_default_exit_command();
    }

    void run(ShellEnv& env) {
        env.shell_streams().out << PreambleString;
        _shell.run(env);
    }
};

} // namespace ld

#endif//LAMBDA_SHELL_SHELL_HPP