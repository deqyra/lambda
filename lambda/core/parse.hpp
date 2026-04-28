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

using SyntaxTree = tools::tree<SyntaxNode>;
using SyntaxNodeHandle = SyntaxTree::node_handle_t;
using SyntaxConstNodeHandle = SyntaxTree::const_node_handle_t;

SyntaxTree parse_expression(ParseContext& ctx);
SyntaxTree parse_abstraction(ParseContext& ctx);
SyntaxTree parse_application(ParseContext& ctx);
SyntaxTree parse_identifier(ParseContext& ctx);
SyntaxTree parse_variable(ParseContext& ctx);

std::string minimal_source_from_syntax_subtree(SyntaxConstNodeHandle syntax_subtree);
std::size_t rebind_text_from_minimal_source(std::string_view source, SyntaxNodeHandle syntax_subtree);

} // namespace ld

#endif//LAMBDA_PARSE_HPP