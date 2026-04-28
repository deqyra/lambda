#ifndef LAMBDA_SHELL_COMMANDS_INSPECT_COMMAND_HPP
#define LAMBDA_SHELL_COMMANDS_INSPECT_COMMAND_HPP

#include <format>
#include <ranges>

#include <cpptools/utility/string.hpp>

#include <lambda/shell/common.hpp>

namespace ld::shell {

namespace {

    std::string expand_definition(std::string_view id, const AST& expression, const DeclarationMap& decls) {
        std::string result;

        const auto& [expr, text] = expression.tree().root().value();
        const auto* identifier = std::get_if<Identifier>(&expr);

        // recursively try to expand expressions which consist of a single identifier
        if (identifier) {
            auto it = decls.find(text);
            if (it != decls.end()) {
                auto expansion = expand_definition(text, it->second, decls);
                return std::format("{} → {}", id, expansion);
            }
        }

        return std::format("{} → {}", id, text);
    }

}

class InspectCommand : public shell_t::command {
    code run(std::string_view command, ShellEnv& env) override {
        namespace stdv = std::views;
        auto& s = env.shell_streams();

        auto tokens = tools::tokenize(command, is_space, /*discard_empty=*/true);
        std::string undeclared_ids;

        auto args = tokens | stdv::drop(1); // skip command name
        for (const auto& id : args) {
            auto it = env.declarations.find(id);

            if (it != env.declarations.end()) {
                s.out << expand_definition(id, it->second, env.declarations) << '\n';
            } else {
                undeclared_ids += std::format(", {}", id); // this prepends an initial ", " to the list of identifiers
            }
        }

        if (not undeclared_ids.empty()) {
            auto skipped_decls = std::string_view(undeclared_ids);
            skipped_decls.remove_prefix(2); // remove initial ", "
            s.out << std::format(
                "The following identifiers did not match any known declaration and were not expanded: {}\n", skipped_decls
            );
        }

        return code::success;
    }
};

} // namespace ld

#endif//LAMBDA_SHELL_COMMANDS_INSPECT_COMMAND_HPP