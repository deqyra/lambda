#include <catch2/catch_test_macros.hpp>

#include <lambda/core/token.hpp>
#include <string_view>

#define TAGS "[token]"

namespace ld {

using namespace std::string_view_literals;
using enum ETokenKind;

TEST_CASE("Token::Kind::from_char", TAGS) {

    constexpr auto IdentifierChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-*$+/~@"sv;
    for (char c : IdentifierChars) {
        REQUIRE(Token::Kind::from_char(c) == Identifier);
        REQUIRE(Token::Kind(c)            == Identifier);
    }

    constexpr auto WhitespaceChars = " \t\r\n\f\v"sv;
    for (char c : WhitespaceChars) {
        REQUIRE(Token::Kind::from_char(c) == Whitespace);
        REQUIRE(Token::Kind(c)            == Whitespace);
    }

    REQUIRE(Token::Kind::from_char('\\') == Lambda);
    REQUIRE(Token::Kind('\\')            == Lambda);
    REQUIRE(Token::Kind::from_char('.')  == Dot);
    REQUIRE(Token::Kind('.')             == Dot);
    REQUIRE(Token::Kind::from_char('(')  == LeftParenthesis);
    REQUIRE(Token::Kind('(')             == LeftParenthesis);
    REQUIRE(Token::Kind::from_char(')')  == RightParenthesis);
    REQUIRE(Token::Kind(')')             == RightParenthesis);

    static constexpr std::string_view InvalidChars = "[]{}`=^&:";
    for (char c : InvalidChars) {
        REQUIRE(Token::Kind::from_char(c) == Invalid);
        REQUIRE(Token::Kind(c)            == Invalid);
    }
}

TEST_CASE("tokenize - empty string", TAGS) {
    REQUIRE(tokenize("") == TokenizedSourceView{});
}

TEST_CASE("tokenize - single-character tokens", TAGS) {
    constexpr auto Source = " q\t.E\\)\r*(\f1\v_"sv;

    const auto Expected = TokenizedSourceView{
        .text = Source,
        .tokens = {
            { Whitespace,       Source.substr( 0, 1) },
            { Identifier,       Source.substr( 1, 1) },
            { Whitespace,       Source.substr( 2, 1) },
            { Dot,              Source.substr( 3, 1) },
            { Identifier,       Source.substr( 4, 1) },
            { Lambda,           Source.substr( 5, 1) },
            { RightParenthesis, Source.substr( 6, 1) },
            { Whitespace,       Source.substr( 7, 1) },
            { Identifier,       Source.substr( 8, 1) },
            { LeftParenthesis,  Source.substr( 9, 1) },
            { Whitespace,       Source.substr(10, 1) },
            { Identifier,       Source.substr(11, 1) },
            { Whitespace,       Source.substr(12, 1) },
            { Identifier,       Source.substr(13, 1) }
        }
    };

    REQUIRE(tokenize(Source) == Expected);
}

TEST_CASE("tokenize - multi-character tokens", TAGS) {
    constexpr auto Source = "\t \\func. \f \\var.  (f   x)\r\n"sv;

    const auto Expected = TokenizedSourceView{
        .text = Source,
        .tokens = {
            { Whitespace,       Source.substr( 0, 2) },
            { Lambda,           Source.substr( 2, 1) },
            { Identifier,       Source.substr( 3, 4) },
            { Dot,              Source.substr( 7, 1) },
            { Whitespace,       Source.substr( 8, 3) },
            { Lambda,           Source.substr(11, 1) },
            { Identifier,       Source.substr(12, 3) },
            { Dot,              Source.substr(15, 1) },
            { Whitespace,       Source.substr(16, 2) },
            { LeftParenthesis,  Source.substr(18, 1) },
            { Identifier,       Source.substr(19, 1) },
            { Whitespace,       Source.substr(20, 3) },
            { Identifier,       Source.substr(23, 1) },
            { RightParenthesis, Source.substr(24, 1) },
            { Whitespace,       Source.substr(25, 2) }
        }
    };

    REQUIRE(tokenize(Source) == Expected);
}

}
