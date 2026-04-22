#include "test_expr.hpp"
#include <catch2/catch_test_macros.hpp>

#include <ranges>

#include <cpptools/container/tree/traversal.hpp>

#include <lambda/core/parse.hpp>
#include <lambda/core/parse_context.hpp>
#include <lambda/core/parse_error.hpp>
#include <lambda/core/token.hpp>

#include <tests/ostream_dump.hpp>
#include <tests/core/expression_library.hpp>

#define TAGS "[parse]"

namespace ld {

using parse_func = tools::tree<SyntaxNode>(ParseContext&);

void test_parse(parse_func parse, const expr::TestExpr& expr, bool ctx_ends_empty) {
    auto ctx = ParseContext(expr.tokenized_source);
    auto result = parse(ctx);
    REQUIRE(result == expr.ast.tree);

    if (ctx_ends_empty) {
        REQUIRE(ctx.empty());
    } else {
        REQUIRE_FALSE(ctx.empty());
    }
}

TEST_CASE("parse_identifier - nominal", TAGS) {
    const auto& Id = expr::Identifier;
    auto ctx = ParseContext(Id.tokenized_source);

    REQUIRE(parse_identifier(ctx) == Id.ast.tree);
    REQUIRE(ctx.empty());
}

TEST_CASE("parse_identifier - empty", TAGS) {
    auto ctx = ParseContext(TokenizedSourceView{});

    REQUIRE_THROWS_AS(parse_identifier(ctx), parse_error);
}

TEST_CASE("parse_identifier - extra spaces", TAGS) {
    const auto& Id = expr::IdentifierExtraSpaces;
    auto ctx = ParseContext(Id.tokenized_source);

    REQUIRE(parse_identifier(ctx) == Id.ast.tree);
    REQUIRE_FALSE(ctx.empty());

    ctx.exhaust_whitespace();
    REQUIRE(ctx.empty());
}

TEST_CASE("parse_identifier - invalid", TAGS) {
    const auto& Source = expr::InvalidIdentifier.tokenized_source;
    auto ctx = ParseContext(Source);

    REQUIRE_THROWS_AS(parse_identifier(ctx), parse_error);
    REQUIRE_FALSE(ctx.empty());
    REQUIRE(ctx.peek() == Token{ ETokenKind::Invalid, Source.text });
}

TEST_CASE("parse_variable - nominal", TAGS) {
    const auto& Var = expr::Variable;
    auto ctx = ParseContext(Var.tokenized_source);

    REQUIRE(parse_variable(ctx) == Var.ast.tree);
    REQUIRE(ctx.empty());
}

TEST_CASE("parse_variable - empty", TAGS) {
    auto ctx = ParseContext(TokenizedSourceView{});

    REQUIRE_THROWS_AS(parse_variable(ctx), parse_error);
}

TEST_CASE("parse_variable - extra spaces", TAGS) {
    const auto& Var = expr::VariableExtraSpaces;
    auto ctx = ParseContext(Var.tokenized_source);

    REQUIRE(parse_variable(ctx) == Var.ast.tree);
    REQUIRE_FALSE(ctx.empty());

    ctx.exhaust_whitespace();
    REQUIRE(ctx.empty());
}

TEST_CASE("parse_variable - invalid", TAGS) {
    const auto& Source = expr::InvalidIdentifier.tokenized_source;
    auto ctx = ParseContext(Source);

    REQUIRE_THROWS_AS(parse_variable(ctx), parse_error);
    REQUIRE_FALSE(ctx.empty());
    REQUIRE(ctx.peek() == Token{ ETokenKind::Invalid, Source.text });
}

TEST_CASE("parse_abstraction - nominal", TAGS) {
    const auto& Abs = expr::Abstraction;
    auto ctx = ParseContext(Abs.tokenized_source);

    REQUIRE(parse_abstraction(ctx) == Abs.ast.tree);
    REQUIRE(ctx.empty());
}

TEST_CASE("parse_abstraction - empty", TAGS) {
    auto ctx = ParseContext(TokenizedSourceView{});

    REQUIRE_THROWS_AS(parse_abstraction(ctx), parse_error);
}

TEST_CASE("parse_abstraction - extra spaces", TAGS) {
    const auto& Abs = expr::AbstractionExtraSpaces;
    auto ctx = ParseContext(Abs.tokenized_source);

    REQUIRE(parse_abstraction(ctx) == Abs.ast.tree);
    REQUIRE_FALSE(ctx.empty());

    ctx.exhaust_whitespace();
    REQUIRE(ctx.empty());
}

TEST_CASE("parse_abstraction - invalid", TAGS) {
    const auto& Abs = expr::InvalidAbstraction;
    auto ctx = ParseContext(Abs.tokenized_source);

    REQUIRE_THROWS_AS(parse_abstraction(ctx), parse_error);
    REQUIRE_FALSE(ctx.empty());
    REQUIRE(ctx.peek() == Token{ ETokenKind::LeftParenthesis, Abs.text.substr(2, 1) });
}

TEST_CASE("parse_application - nominal", TAGS) {
    const auto& App = expr::Application;
    auto ctx = ParseContext(App.tokenized_source);

    REQUIRE(parse_application(ctx) == App.ast.tree);
    REQUIRE(ctx.empty());
}

TEST_CASE("parse_application - empty", TAGS) {
    auto ctx = ParseContext(TokenizedSourceView{});

    REQUIRE_THROWS_AS(parse_application(ctx), parse_error);
}

TEST_CASE("parse_application - extra spaces", TAGS) {
    const auto& App = expr::ApplicationExtraSpaces;
    auto ctx = ParseContext(App.tokenized_source);

    REQUIRE(parse_application(ctx) == App.ast.tree);
    REQUIRE_FALSE(ctx.empty());

    ctx.exhaust_whitespace();
    REQUIRE(ctx.empty());
}

TEST_CASE("parse_application - invalid", TAGS) {
    const auto& App = expr::InvalidApplication;
    auto ctx = ParseContext(App.tokenized_source);

    REQUIRE_THROWS_AS(parse_application(ctx), parse_error);
    REQUIRE_FALSE(ctx.empty());
    REQUIRE(ctx.peek() == Token{ ETokenKind::RightParenthesis, App.text.substr(2, 1) });
}

TEST_CASE("parse_expression", TAGS) {
    const expr::TestExpr* exprs[] = {
        &expr::Identifier,
        &expr::IdentifierExtraSpaces,
        &expr::Application,
        &expr::ApplicationExtraSpaces,
        &expr::Abstraction,
        &expr::AbstractionExtraSpaces,
        &expr::One,
        &expr::OneExtraSpaces
    };

    for (const auto* expr : exprs) {
        auto ctx = ParseContext(expr->tokenized_source);
        REQUIRE(parse_expression(ctx) == expr->ast.tree);
    }

    const expr::TestExpr* invalid_exprs[] = {
        &expr::InvalidIdentifier,
        &expr::InvalidApplication,
        &expr::InvalidAbstraction
    };

    for (const auto* expr : invalid_exprs) {
        auto ctx = ParseContext(expr->tokenized_source);
        REQUIRE_THROWS_AS(parse_expression(ctx), parse_error);
        REQUIRE_FALSE(ctx.empty());
    }

    auto ctx = ParseContext(TokenizedSourceView{});
    REQUIRE_THROWS_AS(parse_expression(ctx), parse_error);
}

TEST_CASE("minimal_source_from_syntax_subtree", TAGS) {
    const expr::TestExpr* exprs[] = {
        &expr::Identifier,
        &expr::Application,
        &expr::Abstraction,
        &expr::One,
    };

    for (const auto* expr : exprs) {
        REQUIRE(minimal_source_from_syntax_subtree(expr->ast.tree.root()) == expr->ast.source);
    }
}

TEST_CASE("rebind_text_from_minimal_source - nominal", TAGS) {
    const expr::TestExpr* exprs[] = {
        &expr::Identifier,
        &expr::Application,
        &expr::Abstraction,
        &expr::One,
    };

    for (const auto* expr : exprs) {
        std::string source = expr->ast.source;
        auto tree = expr->ast.tree;
        rebind_text_from_minimal_source(source, tree.root());

        auto orig_begin = expr->ast.source.c_str();
        auto copy_begin = source.c_str();

        using namespace tools::traversal;
        namespace stdv = std::views;
        for (const auto& [base_node, rebound_node]: stdv::zip(dfs<pre_order>(expr->ast.tree), dfs<pre_order>(tree))) {
            // Rebound node must be equivalent to base node...
            REQUIRE(rebound_node.expr == base_node.expr);
            REQUIRE(rebound_node.text == base_node.text);

            // ... but point into provided source
            auto orig_offset = base_node.text.data() - orig_begin;
            auto copy_offset = rebound_node.text.data() - copy_begin;
            REQUIRE(copy_offset == orig_offset);
        }
    }
}

TEST_CASE("make_minimal_ast", TAGS) {
    auto ctx = ParseContext(expr::ApplicationExtraSpaces.tokenized_source);
    auto tree = parse_expression(ctx);
    auto ast = make_minimal_ast(std::move(tree));

    REQUIRE(ast.source == expr::Application.ast.source);

    auto orig_begin = expr::Application.ast.source.c_str();
    auto copy_begin = ast.source.c_str();

    using namespace tools::traversal;
    namespace stdv = std::views;
    for (const auto& [base_node, copy_node]: stdv::zip(dfs<pre_order>(expr::Application.ast.tree), dfs<pre_order>(ast.tree))) {
        // Rebound node must be equivalent to base node...
        REQUIRE(copy_node.expr == base_node.expr);
        REQUIRE(copy_node.text == base_node.text);

        // ... but point into provided source
        auto orig_offset = base_node.text.data() - orig_begin;
        auto copy_offset = copy_node.text.data() - copy_begin;
        REQUIRE(copy_offset == orig_offset);
    }
}

TEST_CASE("parse_full_expression - minimal expression", TAGS) {
    auto result = parse_full_expression(expr::One.tokenized_source);
    REQUIRE(result == expr::One.ast);
}

TEST_CASE("parse_full_expression - expression with extra spaces", TAGS) {
    auto result = parse_full_expression(expr::OneExtraSpaces.tokenized_source);
    REQUIRE(result == expr::One.ast);
}

}
