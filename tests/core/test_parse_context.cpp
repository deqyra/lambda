#include <catch2/catch_test_macros.hpp>

#include <lambda/core/parse_context.hpp>
#include <lambda/core/parse_error.hpp>

#include "expression_library.hpp"
#include "lambda/core/token.hpp"

#define TAGS "[parse_context]"

namespace ld {

TEST_CASE("parse_context - initial state", TAGS) {
    const auto& source = expr::OneExtraSpaces.tokenized_source;
    auto ctx = ParseContext(source);

    REQUIRE(ctx.text_pos() == 0);
    REQUIRE(ctx.token_pos() == 0);
    REQUIRE(ctx.data() == expr::OneExtraSpaces.tokenized_source.text.data());
}

TEST_CASE("parse_context - peek", TAGS) {
    const auto& source = expr::OneExtraSpaces.tokenized_source;
    auto ctx = ParseContext(source);

    for (std::size_t i = 0; i < source.tokens.size(); ++i) {
        REQUIRE_FALSE(ctx.empty());
        REQUIRE(ctx.peek() == ctx.peek(0));

        for (std::size_t j = 0; j < source.tokens.size() - i; ++j) {
            REQUIRE(ctx.peek(j) == source.tokens[i + j]);
        }

        ctx.advance();
    }

    REQUIRE(ctx.empty());
    REQUIRE_THROWS_AS(ctx.peek(), parse_error);
}

TEST_CASE("parse_context - advance / empty", TAGS) {
    const auto& source = expr::OneExtraSpaces.tokenized_source;
    auto ctx = ParseContext(source);

    for (std::size_t i = 0; i < source.tokens.size(); ++i) {
        REQUIRE(ctx.token_pos() == i);
        REQUIRE_FALSE(ctx.empty());
        ctx.advance();
    }

    REQUIRE(ctx.empty());
    REQUIRE_NOTHROW(ctx.advance());
}

TEST_CASE("parse_context - next", TAGS) {
    const auto& source = expr::OneExtraSpaces.tokenized_source;
    auto ctx = ParseContext(source);

    for (std::size_t i = 0; i < source.tokens.size(); ++i) {
        REQUIRE(ctx.next() == source.tokens[i]);
    }

    REQUIRE(ctx.empty());
    REQUIRE_THROWS_AS(ctx.next(), parse_error);
}

TEST_CASE("parse_context - exhaust_whitespace") {
    const auto& source = expr::OneExtraSpaces.tokenized_source;
    auto ctx = ParseContext(source);

    for (std::size_t i = 0; i < source.tokens.size(); ++i) {
        if (ctx.peek().kind == ETokenKind::Whitespace) {
            ctx.exhaust_whitespace();
            REQUIRE(ctx.token_pos() == i + 1);
            // i + 1 is wrong when there is more than one consecutive whitespace token, which
            // should never happen (tokenize step aggregates them together)
        } else {
            ctx.exhaust_whitespace(); // should do nothing
            REQUIRE(ctx.token_pos() == i);
            ctx.advance();
        }
    }

    REQUIRE(ctx.empty());
    REQUIRE_NOTHROW(ctx.exhaust_whitespace());
}

TEST_CASE("parse_context - data / text_pos / token_pos") {
    const auto& source = expr::OneExtraSpaces.tokenized_source;
    auto ctx = ParseContext(source);
    std::size_t accumulated_text_pos = 0;

    for (std::size_t i = 0; i < source.tokens.size(); ++i) {
        REQUIRE(ctx.data() == source.text.data() + accumulated_text_pos);
        REQUIRE(ctx.text_pos() == accumulated_text_pos);
        REQUIRE(ctx.token_pos() == i);

        accumulated_text_pos += ctx.peek().text.size();

        ctx.advance();
    }

    REQUIRE(ctx.empty());
    REQUIRE(ctx.data() == source.text.data() + accumulated_text_pos);
    REQUIRE(ctx.text_pos() == accumulated_text_pos);
    REQUIRE(ctx.token_pos() == source.tokens.size());
}

TEST_CASE("parse_context - expect") {
    const auto& source = expr::OneExtraSpaces.tokenized_source;
    auto ctx = ParseContext(source);

    for (std::size_t i = 0; i < source.tokens.size(); ++i) {
        REQUIRE_NOTHROW(ctx.expect(source.tokens[i].kind));
        ctx.advance();
    }

    REQUIRE(ctx.empty());
    REQUIRE_THROWS_AS(ctx.expect(ETokenKind::Lambda),           parse_error);
    REQUIRE_THROWS_AS(ctx.expect(ETokenKind::Dot),              parse_error);
    REQUIRE_THROWS_AS(ctx.expect(ETokenKind::LeftParenthesis),  parse_error);
    REQUIRE_THROWS_AS(ctx.expect(ETokenKind::RightParenthesis), parse_error);
    REQUIRE_THROWS_AS(ctx.expect(ETokenKind::Identifier),       parse_error);
    REQUIRE_THROWS_AS(ctx.expect(ETokenKind::Whitespace),       parse_error);
    REQUIRE_THROWS_AS(ctx.expect(ETokenKind::Invalid),          parse_error);
}

TEST_CASE("parse_context - expect_and_consume") {
    const auto& source = expr::OneExtraSpaces.tokenized_source;
    auto ctx = ParseContext(source);

    for (std::size_t i = 0; i < source.tokens.size(); ++i) {
        REQUIRE_NOTHROW(ctx.expect_and_consume(source.tokens[i].kind));
    }

    REQUIRE(ctx.empty());
    REQUIRE_THROWS_AS(ctx.expect_and_consume(ETokenKind::Lambda),           parse_error);
    REQUIRE_THROWS_AS(ctx.expect_and_consume(ETokenKind::Dot),              parse_error);
    REQUIRE_THROWS_AS(ctx.expect_and_consume(ETokenKind::LeftParenthesis),  parse_error);
    REQUIRE_THROWS_AS(ctx.expect_and_consume(ETokenKind::RightParenthesis), parse_error);
    REQUIRE_THROWS_AS(ctx.expect_and_consume(ETokenKind::Identifier),       parse_error);
    REQUIRE_THROWS_AS(ctx.expect_and_consume(ETokenKind::Whitespace),       parse_error);
    REQUIRE_THROWS_AS(ctx.expect_and_consume(ETokenKind::Invalid),          parse_error);
}

}