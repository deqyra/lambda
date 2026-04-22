#ifndef LAMBDA_PARSE_HPP
#define LAMBDA_PARSE_HPP

#include <string_view>
#include <variant>

#include <cpptools/container/tree.hpp>

#include <lambda/core/parse_context.hpp>
#include <lambda/core/parse_error.hpp>
#include <lambda/core/token.hpp>

namespace ld {

struct Abstraction {
    friend bool operator==(const Abstraction& left, const Abstraction& right) = default;
};
struct Application {
    friend bool operator==(const Application& left, const Application& right) = default;
};
struct Identifier {
    friend bool operator==(const Identifier& left, const Identifier& right) = default;
};
struct Variable {
    friend bool operator==(const Variable& left, const Variable& right) = default;
};

struct SyntaxNode {
    using LexicalExpression = std::variant<Abstraction, Application, Identifier, Variable>;

    LexicalExpression expr;
    std::string_view text;

    friend bool operator==(const SyntaxNode& left, const SyntaxNode& right) = default;
};

// abstract syntax tree
struct AST {
    std::string_view source;
    tools::tree<SyntaxNode> tree;

    friend bool operator==(const AST& left, const AST& right) = default;
};

tools::tree<SyntaxNode> parse_expression(ParseContext& ctx);
tools::tree<SyntaxNode> parse_abstraction(ParseContext& ctx);
tools::tree<SyntaxNode> parse_application(ParseContext& ctx);
tools::tree<SyntaxNode> parse_identifier(ParseContext& ctx);
tools::tree<SyntaxNode> parse_variable(ParseContext& ctx);

AST parse_full_expression(const TokenizedSourceView& source);

} // namespace ld

#endif//LAMBDA_PARSE_HPP