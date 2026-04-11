#include <catch2/catch_test_macros.hpp>

#include <lambda/core/parse.hpp>

#include "expression_library.hpp"

#define TAGS "[parse]"

using namespace std::string_view_literals;
using namespace ld;
using enum ETokenKind;

TEST_CASE("parse_full_expression", TAGS) {
    auto result = parse_full_expression(expr::One.tokenizedSource);
    REQUIRE(result == expr::One.ast);
}