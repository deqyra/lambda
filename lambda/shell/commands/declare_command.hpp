#ifndef LAMBDA_SHELL_COMMANDS_DECLARE_COMMAND_HPP
#define LAMBDA_SHELL_COMMANDS_DECLARE_COMMAND_HPP


#include <cpptools/exception/internal_exception.hpp>
#include <cpptools/utility/string.hpp>

#include <exception>
#include <lambda/core/parse.hpp>
#include <lambda/core/parse_error.hpp>
#include <lambda/core/token.hpp>
#include <lambda/shell/declare.hpp>
#include <lambda/shell/forceable_command.hpp>
#include <urlmon.h>

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
        using enum declare_status::ec;
        auto& s = env.shell_streams();
        auto [error_code, id, ex] = declare(command, force, env.declarations);

        switch (error_code) {

        case missing_body:
            s.err << "Invalid declaration: body not found\n";
            return code::bad_arguments;

        case invalid_identifier:
            s.err << std::format("Invalid declaration: '{}' is not a valid identifier\n", *id);
            return code::bad_arguments;

        case identifier_already_exists:
            s.err << std::format("Invalid declaration: identifier '{}' is already declared, use let! to override\n", *id);
            return code::failure;

        case invalid_body:
            try {
                std::rethrow_exception(ex);
            } catch (const std::exception& e) {
                s.err << std::format("Invalid declaration: failed to parse body - {}", nested_exception_to_string(e));
            }
            return code::bad_arguments;

        case ok:
            return code::success;

        default:
            CPPTOOLS_THROW(tools::exception::internal::not_implemented_error);
        }
    }
};

} // namespace ld

#endif//LAMBDA_SHELL_COMMANDS_DECLARE_COMMAND_HPP