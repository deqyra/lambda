#ifndef LAMBDA_SHELL_COMMANDS_DECLARE_COMMAND_HPP
#define LAMBDA_SHELL_COMMANDS_DECLARE_COMMAND_HPP

#include <algorithm>

#include <cpptools/utility/string.hpp>

#include <lambda/core/parse.hpp>
#include <lambda/core/parse_error.hpp>
#include <lambda/core/token.hpp>
#include <lambda/shell/forceable_command.hpp>

namespace ld::shell {

class DeclareCommand : public ForceableCommand {
public:
    DeclareCommand()
        : ForceableCommand("let")
    {

    }

   static constexpr std::string_view HelpText =
R"(let - declare a named expression

Syntax:
    let  <id>: <expr> - declare <id> as a name for expression <expr>
    let! <id>: <expr> - same but no error if <id> is declared

<id>   - identifier of the named expression
<expr> - lambda expression to bind <id> to

When <id> is already declared, the base command returns an error and <id> is not rebound. The bang version of the
command can be used to instead silently rebind <id> to <expr>.

A valid identifier consists of alphanumeric characters and any of the following:
_ - * $ + / ~ @
Identifiers are case-sentive.

A valid lambda expression, informally, is any one of the following:
- Abstraction:     \{var}.{expr}
- Application:     ({expr} {expr})
- Variable:        {var}
where {expr} can be any such valid expression, and {var} can be any valid identifier (*).

(*): An expression may not refer to itself. Consequently, a free variable in the full expression may not have the same
name as the declared identifier.
)";

    code run(std::string_view command, bool force, ShellEnv& env) override {
        namespace stdr = std::ranges;
        auto& s = env.shell_streams();

        auto colon_pos = command.find(':');
        if (colon_pos == std::string_view::npos) {
            s.err << "Invalid declaration: body not found\n";
            return code::bad_arguments;
        }

        auto identifier = command.substr(0, colon_pos);
        if (identifier.size() == 0 or not stdr::all_of(identifier, is_identifier_char)) {
            s.err << std::format("Invalid declaration: '{}' is not a valid identifier\n", identifier);
            return code::bad_arguments;
        }

        if (not force and env.declarations.find(identifier) != env.declarations.end()) {
            s.err << std::format("Invalid declaration: identifier '{}' is already declared, use let! to override\n", identifier);
            return code::failure;
        }

        auto body = command.substr(colon_pos + 1);

        AST expression;
        try {
            expression = parse_full_expression(tokenize(body));
        } catch (const parse_error& e) {
            s.err << std::format("Invalid declaration: failed to parse body - {}", nested_exception_to_string(e));
            return code::bad_arguments;
        }

        env.declarations.insert({std::string(identifier), std::move(expression)});

        return code::success;
    }
};

} // namespace ld

#endif//LAMBDA_SHELL_COMMANDS_DECLARE_COMMAND_HPP