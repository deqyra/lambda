#ifndef LAMBDA_SHELL_COMMANDS_EXPORT_COMMAND_HPP
#define LAMBDA_SHELL_COMMANDS_EXPORT_COMMAND_HPP

#include <fstream>
#include <filesystem>
#include <ostream>
#include <ranges>

#include <cpptools/utility/string.hpp>

#include <lambda/core/token.hpp>
#include <lambda/shell/forceable_command.hpp>

namespace ld::shell {

class ExportCommand : public ForceableCommand {
public:
    ExportCommand()
        : ForceableCommand("export")
    {

    }

   static constexpr std::string_view HelpText =
R"(export - save declared identifiers to a file

Syntax:
    export  <file> [<ids...>] - save <ids...> to <file>
    export! <file> [<ids...>] - same but silently overwrite <file> if it exists

<file>   - path to a file to write definitions to
<ids...> - space-separated list of identifiers whose definition is to be saved
           optional: if not specified, all declared identifiers will be saved

If <file> already exists, the base command returns an error and the export is aborted. The bang version of the command
can be used to instead silently overwrite it.
)";

    code run(std::string_view command, bool force, ShellEnv& env) override {
        namespace stdv = std::views;
        auto& s = env.shell_streams();

        auto args = tools::tokenize(command, is_space, /*discard_empty=*/true);
        if (args.size() < 1) {
            s.err << "export: too few arguments\n";
            s.out << HelpText;
            return code::bad_arguments;
        }

        auto out_file_path = std::filesystem::path(args[0]);

        if (not force and std::filesystem::exists(out_file_path)) {
            s.err << std::format("export: file {} already exists, use export! to overwrite it", out_file_path.string());
            return code::bad_arguments;
        }

        auto file = std::ofstream(out_file_path, std::ios::out | std::ios::trunc);
        if (!file) {
            s.err << std::format("export: cannot open file {} for writing\n", args[0]);
            return code::failure;
        }

        struct {
            std::size_t ok = 0;
            std::string unkown;
        } status;

        if (args.size() == 1) {
            for (const auto& [id, ast]: env.declarations) {
                file << make_declaration(id, ast);
                ++status.ok;
            }
        } else {
            for (auto id : args | stdv::drop(1)) {
                auto it = env.declarations.find(id);
                if (it == env.declarations.end()) {
                    status.unkown += std::format(", {}", id); // this prepends an initial ", " to the list
                } else {
                    file << make_declaration(id, it->second);
                    ++status.ok;
                }
            }
        }

        file << std::flush;

        s.out << std::format("Exported {} declarations.\n", status.ok);
        if (not status.unkown.empty()) {
            auto skipped_decls = std::string_view(status.unkown);
            skipped_decls.remove_prefix(2); // remove initial ", "
            s.out << std::format(
                "The following identifiers did not match any known declaration and were not exported: {}\n",
                skipped_decls
            );
        }

        return code::success;
    }

private:
    std::string make_declaration(std::string_view identifier, AST expression) {
        return std::format("let {}: {}\n", identifier, expression.tree.root()->text);
    }
};

} // namespace ld

#endif//LAMBDA_SHELL_COMMANDS_EXPORT_COMMAND_HPP