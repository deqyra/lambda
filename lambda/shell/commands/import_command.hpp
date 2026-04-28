#ifndef LAMBDA_SHELL_COMMANDS_IMPORT_COMMAND_HPP
#define LAMBDA_SHELL_COMMANDS_IMPORT_COMMAND_HPP

#include <fstream>
#include <filesystem>

#include <cpptools/utility/string.hpp>

#include <lambda/core/token.hpp>
#include <lambda/shell/declare.hpp>
#include <lambda/shell/forceable_command.hpp>

namespace ld::shell {

class ImportCommand : public ForceableCommand {
public:
    ImportCommand()
        : ForceableCommand("import")
    {

    }

   static constexpr std::string_view HelpText =
R"(import - import declarations from a file

Syntax:
    import  <file> [<ids...>] - import definitions from <file>
    import! <file> [<ids...>] - same but silently override existing declarations

<file>   - path to a file to read definitions from
<ids...> - space-separated list of identifiers to import from <file>
           optional: if not specified, all identifiers in <file> will be imported

If the name of an imported identifier is already used by an existing declaration, the base command does not import it
and leaves the existing declaration untouched. The bang version of the command can be used to instead silently override
existing declarations with the imported ones.
)";

    code run(std::string_view command, bool force, ShellEnv& env) override {
        auto& s = env.shell_streams();

        auto args = tools::tokenize(command, is_space, true);
        if (args.size() < 1) {
            s.err << "import: too few arguments\n";
            s.out << HelpText;
            return code::bad_arguments;
        }

        auto file = std::ifstream(std::filesystem::path(args[0]));
        if (!file) {
            s.err << std::format("import: cannot open file {} for reading\n", args[0]);
            return code::failure;
        }

        struct {
            std::size_t ok = 0;
            std::size_t invalid = 0;
            std::string already_existing_ids;
        } status;

        std::string line;
        while (std::getline(file, line)) {
            auto decl = tools::trim(std::string_view(line));
            if (decl.empty()) {
                continue;
            }

            std::string_view declaration = line;
            declaration = tools::trim(declaration);
            if (not declaration.starts_with("let")) {
                continue;
            }

            declaration.remove_prefix(3); // sizeof("let")
            declaration = tools::trim(declaration);

            auto [error_code, id, ex] = declare(declaration, force, env.declarations);

            switch (error_code) {
            using enum declare_status::ec;
            case missing_body:
            case invalid_body:
            case invalid_identifier:
                ++status.invalid;
                break;

            case identifier_already_exists:
                status.already_existing_ids += std::format(", {}", *id); // this prepends an initial ", " to the list
                break;

            case ok:
                ++status.ok;
                break;

            default:
                CPPTOOLS_THROW(tools::exception::internal::not_implemented_error);
            }
        }

        s.out << std::format("Imported {} declarations", status.ok);
        if (status.invalid > 0) {
            s.out << std::format(" and ignored {} invalid declarations", status.invalid);
        }
        s.out << ".\n";
        if (not status.already_existing_ids.empty()) {
            auto skipped_decls = std::string_view(status.already_existing_ids);
            skipped_decls.remove_prefix(2); // remove initial ", "
            s.out << std::format(
                "Due to their identifiers already being used, the following declarations were not imported: {}\n",
                skipped_decls
            );
            s.out << "Use import! to override existing declarations with imported declarations.\n";
        }

        return code::success;
    }
};

} // namespace ld

#endif//LAMBDA_SHELL_COMMANDS_IMPORT_COMMAND_HPP