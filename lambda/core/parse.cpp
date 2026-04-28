#include <format>
#include <stdexcept>
#include <string_view>
#include <variant>

#include <cpptools/container/tree.hpp>
#include <cpptools/utility/overloaded_visitor.hpp>

#include <lambda/core/parse.hpp>
#include <lambda/core/parse_context.hpp>
#include <lambda/core/parse_error.hpp>
#include <lambda/core/token.hpp>

namespace ld {

SyntaxTree parse_expression(ParseContext& ctx) {
    using enum ETokenKind;

    SyntaxTree syntax_tree;
    ctx.exhaust_whitespace();
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

SyntaxTree parse_abstraction(ParseContext& ctx) {
    using enum ETokenKind;

    SyntaxTree syntax_tree;
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

SyntaxTree parse_application(ParseContext& ctx) {
    using enum ETokenKind;

    SyntaxTree syntax_tree;
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

SyntaxTree parse_identifier(ParseContext& ctx) {
    const auto id = ctx.expect_and_consume(ETokenKind::Identifier);

    SyntaxTree syntax_tree;
    syntax_tree.emplace_node(syntax_tree.root(), SyntaxNode{ Identifier{}, id.text});

    return syntax_tree;
}

SyntaxTree parse_variable(ParseContext& ctx) {
    const auto id = ctx.expect_and_consume(ETokenKind::Identifier);

    SyntaxTree syntax_tree;
    syntax_tree.emplace_node(syntax_tree.root(), SyntaxNode{ Variable{}, id.text});

    return syntax_tree;
}

std::string minimal_source_from_syntax_subtree(SyntaxConstNodeHandle syntax_subtree) {
    const SyntaxNode& node = syntax_subtree.value();
    tools::overloaded_visitor make_string{
        [&](const Identifier&) -> std::string {
            return std::string(node.text);
        },
        [&](const Variable&) -> std::string {
            // variable nodes should be handled by the Abstraction case and never visited
            throw std::runtime_error("unexpected");
        },
        [&](const Abstraction&) -> std::string {
            return std::format(
                "\\{}.{}",
                syntax_subtree.child(0)->text,
                minimal_source_from_syntax_subtree(syntax_subtree.child(1))
            );
        },
        [&](const Application&) -> std::string {
            return std::format(
                "({} {})",
                minimal_source_from_syntax_subtree(syntax_subtree.child(0)),
                minimal_source_from_syntax_subtree(syntax_subtree.child(1))
            );
        }
    };
    return std::visit(make_string, node.expr);
}

std::size_t rebind_text_from_minimal_source(std::string_view source, SyntaxNodeHandle syntax_subtree) {
    SyntaxNode& node = syntax_subtree.value();
    tools::overloaded_visitor rebind_text{
        [&](const Identifier&) -> std::size_t {
            auto id_size = node.text.size();
            node.text = source.substr(0, id_size);
            source.remove_prefix(id_size); // consume identifier
            return id_size;
        },
        [&](const Variable&) -> std::size_t {
            auto var_size = node.text.size();
            node.text = source.substr(0, var_size);
            source.remove_prefix(var_size); // consume variable name
            return var_size;
        },
        [&](const Abstraction&) -> std::size_t {
            auto original = source;

            source.remove_prefix(1); // consume '\'
            auto var_size = rebind_text_from_minimal_source(source, syntax_subtree.child(0));
            source.remove_prefix(var_size); // consume rebound expression

            source.remove_prefix(1); // consume '.'
            auto body_size = rebind_text_from_minimal_source(source, syntax_subtree.child(1));
            source.remove_prefix(body_size); // consume rebound expression

            auto abs_size = var_size + body_size + 2;
            node.text = original.substr(0, abs_size);

            return abs_size;
        },
        [&](const Application&) -> std::size_t {
            auto original = source;

            source.remove_prefix(1); // consume '('
            auto left_size = rebind_text_from_minimal_source(source, syntax_subtree.child(0));
            source.remove_prefix(left_size); // consume rebound expression

            source.remove_prefix(1); // consume ' '
            auto right_size = rebind_text_from_minimal_source(source, syntax_subtree.child(1));
            source.remove_prefix(right_size); // consume rebound expression

            source.remove_prefix(1); // consume ')'

            auto app_size = left_size + right_size + 3;
            node.text = original.substr(0, app_size);
            return app_size;
        }
    };

    return std::visit(rebind_text, node.expr);
}

} // namespace ld
