#include <ranges>

#include <catch2/catch_test_macros.hpp>

#include <cpptools/container/tree/traversal.hpp>

#include <lambda/core/parse.hpp>
#include <lambda/core/parse_context.hpp>
#include <lambda/core/parse_error.hpp>
#include <lambda/core/token.hpp>

#include <tests/ostream_dump.hpp>
#include <tests/core/expression_library.hpp>
#include <tests/core/test_expr.hpp>

#define TAGS "[ast]"

namespace ld {

using ExprPair = std::pair<const expr::TestExpr*, const expr::TestExpr*>;
    
TEST_CASE("make_minimal_ast", TAGS) {
    ExprPair exprs[] = {
        { &expr::IdentifierExtraSpaces,  &expr::Identifier },
        { &expr::ApplicationExtraSpaces, &expr::Application },
        { &expr::AbstractionExtraSpaces, &expr::Abstraction },
        { &expr::OneExtraSpaces,         &expr::One }
    };

    for (auto [WithSpaces, WithoutSpaces] : exprs) {
        auto ctx = ParseContext(WithSpaces->tokenized_source);
        auto tree_with_spaces = parse_expression(ctx);
        auto computed_ast = AST(std::move(tree_with_spaces));

        REQUIRE(computed_ast == WithoutSpaces->ast);
        // this ensures the trees are the same both in value and structure, and map to an equivalent source
        // but comparing the values is not enough

        using namespace tools::traversal;
        auto zipped_tree_nodes = std::views::zip(
            dfs<pre_order>(WithoutSpaces->ast.tree),
            dfs<pre_order>(computed_ast.tree())
        );

        auto WithoutSpacesBegin = WithoutSpaces->ast.source.data();
        auto computed_begin = computed_ast.source().data();

        for (const auto& [NodeWithoutSpaces, computed_node] : zipped_tree_nodes) {
            // the string view in each computed node must point into the computed minimal source owned by the AST
            // and their view into it must be the same as that of WithoutSpaces's AST tree nodes into its own source
            auto expected_offset = NodeWithoutSpaces.text.data() - WithoutSpacesBegin;
            auto computed_offset = computed_node.text.data() - computed_begin;
            // ^ UB if data() returns something which does not point into the same range as computed_begin does, oops

            REQUIRE(computed_offset == expected_offset);
        }
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
