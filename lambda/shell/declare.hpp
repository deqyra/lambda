#ifndef LAMBDA_SHELL_DECLARE_HPP
#define LAMBDA_SHELL_DECLARE_HPP

#include <exception>

#include <lambda/core/parse.hpp>
#include <lambda/shell/shell_env.hpp>

namespace ld::shell {

/// @brief Struct describing the result of a call to declare
struct declare_status {
    enum struct ec {
        ok,                        // id: empty, ex: empty
        missing_body,              // id: empty, ex: empty
        invalid_identifier,        // id: contains the invalid identifier, ex: empty
        identifier_already_exists, // id: contains the declared identifier, ex: empty
        invalid_body               // id: contains the declared identifier, ex: pointer to the parse_error exception that was thrown
    };

    ec error_code; // The value contained here dictates what to expect in the other members of the struct, see ec
    std::optional<std::string> id;
    std::exception_ptr ex;
};

/// @brief Parse a declaration and bind an identifier to an expression
/// @param declaration The declaration to parse
/// @param force Whether or not an existing declaration using the same
/// identifier should be overriden with the newly parsed one
/// @param decl_map The map of existing declarations to which the newly parsed
/// declaration will be added
/// @return A struct describing whether the processing succeeded or why it failed
/// @see declare_status
inline declare_status declare(std::string_view declaration, bool force, DeclarationMap& decl_map) {
    namespace stdr = std::ranges;
    using enum declare_status::ec;

    auto colon_pos = declaration.find(':');
    if (colon_pos == std::string_view::npos) {
        return { missing_body };
    }

    auto identifier = declaration.substr(0, colon_pos);
    if (identifier.size() == 0 or not stdr::all_of(identifier, is_identifier_char)) {
        return { invalid_identifier, std::string(identifier) };
    }

    auto it = decl_map.find(identifier);
    if (not force and it != decl_map.end()) {
        return { identifier_already_exists, std::string(identifier) };
    }

    auto body = declaration.substr(colon_pos + 1);

    AST ast;
    try {
        ast = parse_full_expression(tokenize(body));
    } catch (const parse_error&) {
        return { invalid_body, std::string(identifier), std::current_exception() };
    }

    // TODO: declared identifier must not appear as a free variable in parsed expression

    if (it == decl_map.end()) {
        std::tie(it, std::ignore) = decl_map.insert({std::string(identifier), AST{}});
    }

    it->second = std::move(ast);
    return { ok };
}

} // namespace ld::shell 

#endif//LAMBDA_SHELL_DECLARE_HPP