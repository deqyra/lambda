#include <string_view>
#include <variant>

#include <cpptools/container/tree.hpp>
#include <cpptools/utility/overloaded_visitor.hpp>

#include <lambda/core/parse.hpp>
#include <lambda/core/parse_context.hpp>
#include <lambda/core/parse_error.hpp>
#include <lambda/core/token.hpp>

namespace ld {

tools::tree<SyntaxNode> parse_expression(ParseContext& ctx) {
    using enum ETokenKind;

    tools::tree<SyntaxNode> syntax_tree;
    const auto& token = ctx.peek();
    try {
        switch (token.kind) {
        case Lambda:          syntax_tree = parse_abstraction(ctx); break;
        case LeftParenthesis: syntax_tree = parse_application(ctx); break;
        case Identifier:      syntax_tree = parse_identifier(ctx);  break;
        default:              throw parse_error(std::format("unexpected token at char {}", ctx.text_pos()), token.text);
        }
    } OR_THROW_NESTED_PARSE_ERROR("unable to parse expression");

    return syntax_tree;
}

tools::tree<SyntaxNode> parse_abstraction(ParseContext& ctx) {
    using enum ETokenKind;

    tools::tree<SyntaxNode> syntax_tree;
    syntax_tree.emplace_node(syntax_tree.root(), SyntaxNode{ Abstraction{}, "" });

    try { // Lambda <expr> Dot <expr>
        const auto begin = ctx.data();
        ctx.expect_and_consume(Lambda);

        ctx.exhaust_whitespace();
        auto variable = parse_variable(ctx);

        ctx.exhaust_whitespace();
        ctx.expect_and_consume(Dot);

        ctx.exhaust_whitespace();
        auto body = parse_expression(ctx);
        const auto end = ctx.data();

        syntax_tree.adopt_subtree(syntax_tree.root(), std::move(variable));
        syntax_tree.adopt_subtree(syntax_tree.root(), std::move(body));
        syntax_tree.root()->text = std::string_view(begin, end);
    } OR_THROW_NESTED_PARSE_ERROR("unable to parse abstraction");

    return syntax_tree;
}

tools::tree<SyntaxNode> parse_application(ParseContext& ctx) {
    using enum ETokenKind;

    tools::tree<SyntaxNode> syntax_tree;
    syntax_tree.emplace_node(syntax_tree.root(), SyntaxNode{ Application{}, "" });

    try { // LeftParenthesis <expr> Whitespace <expr> RightParenthesis
        const auto begin = ctx.data();
        ctx.expect_and_consume(LeftParenthesis);

        ctx.exhaust_whitespace();
        auto left = parse_expression(ctx);

        ctx.expect(Whitespace);

        ctx.exhaust_whitespace();
        auto right = parse_expression(ctx);

        ctx.exhaust_whitespace();
        ctx.expect_and_consume(RightParenthesis);
        const auto end = ctx.data();

        syntax_tree.adopt_subtree(syntax_tree.root(), std::move(left));
        syntax_tree.adopt_subtree(syntax_tree.root(), std::move(right));
        syntax_tree.root()->text = std::string_view(begin, end);
    } OR_THROW_NESTED_PARSE_ERROR("unable to parse application");

    return syntax_tree;
}

tools::tree<SyntaxNode> parse_identifier(ParseContext& ctx) {
    const auto id = ctx.expect_and_consume(ETokenKind::Identifier);

    tools::tree<SyntaxNode> syntax_tree;
    syntax_tree.emplace_node(syntax_tree.root(), SyntaxNode{ Identifier{}, id.text});

    return syntax_tree;
}

tools::tree<SyntaxNode> parse_variable(ParseContext& ctx) {
    const auto id = ctx.expect_and_consume(ETokenKind::Identifier);

    tools::tree<SyntaxNode> syntax_tree;
    syntax_tree.emplace_node(syntax_tree.root(), SyntaxNode{ Variable{}, id.text});

    return syntax_tree;
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
