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

tools::tree<SyntaxNode> parse_expression(ParseContext& ctx) {
    using enum ETokenKind;

    tools::tree<SyntaxNode> expr;
    const auto& token = ctx.peek();
    try {
        switch (token.kind) {
        case Lambda:          expr = parse_abstraction(ctx); break;
        case LeftParenthesis: expr = parse_application(ctx); break;
        case Identifier:      expr = parse_identifier(ctx);  break;
        default:              throw parse_error(std::format("unexpected token at char {}", ctx.text_pos()), token.text);
        }
    } OR_THROW_NESTED_PARSE_ERROR("unable to parse expression");

    return expr;
}

tools::tree<SyntaxNode> parse_abstraction(ParseContext& ctx) {
    using enum ETokenKind;

    tools::tree<SyntaxNode> expr;
    expr.emplace_node(expr.root(), SyntaxNode{ Abstraction{}, "" });

    ctx.expect_and_consume(Lambda);
    try {
        ctx.exhaust_whitespace();
        auto variable = parse_variable(ctx);

        ctx.exhaust_whitespace();
        ctx.expect_and_consume(Dot);

        ctx.exhaust_whitespace();
        auto body = parse_expression(ctx);

        expr.adopt_subtree(expr.root(), std::move(variable));
        expr.adopt_subtree(expr.root(), std::move(body));
    } OR_THROW_NESTED_PARSE_ERROR("unable to parse abstraction");

    return expr;
}

tools::tree<SyntaxNode> parse_application(ParseContext& ctx) {
    using enum ETokenKind;

    tools::tree<SyntaxNode> expr;
    expr.emplace_node(expr.root(), SyntaxNode{ Application{}, "" });

    ctx.expect_and_consume(LeftParenthesis);

    try {
        ctx.exhaust_whitespace();
        auto left = parse_expression(ctx);

        ctx.expect(Whitespace);

        ctx.exhaust_whitespace();
        auto right = parse_expression(ctx);

        ctx.exhaust_whitespace();
        ctx.expect_and_consume(RightParenthesis);

        expr.adopt_subtree(expr.root(), std::move(left));
        expr.adopt_subtree(expr.root(), std::move(right));
    } OR_THROW_NESTED_PARSE_ERROR("unable to parse application");

    return expr;
}

tools::tree<SyntaxNode> parse_identifier(ParseContext& ctx) {
    const auto id = ctx.expect_and_consume(ETokenKind::Identifier);

    tools::tree<SyntaxNode> expr;
    expr.emplace_node(expr.root(), SyntaxNode{ Identifier{}, id.text});

    return expr;
}

tools::tree<SyntaxNode> parse_variable(ParseContext& ctx) {
    const auto id = ctx.expect_and_consume(ETokenKind::Identifier);

    tools::tree<SyntaxNode> expr;
    expr.emplace_node(expr.root(), SyntaxNode{ Variable{}, id.text});

    return expr;
}

AST parse_full_expression(const TokenizedSourceView& source) {
    using enum ETokenKind;

    ParseContext ctx(source);
    ctx.exhaust_whitespace();

    if (ctx.empty()) {
        return {
            .source = source.text,
            .tree = {}
        };
    }

    tools::tree<SyntaxNode> expr = parse_expression(ctx);

    ctx.exhaust_whitespace();
    if (not ctx.empty()) {
        throw parse_error("unexpected tokens past end of full expression at char {}", ctx.text_pos());
    }

    return {
        .source = source.text,
        .tree = std::move(expr)
    };
}

} // namespace ld

#endif//LAMBDA_PARSE_HPP