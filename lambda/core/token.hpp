#ifndef LAMBDA_TOKENIZE_HPP
#define LAMBDA_TOKENIZE_HPP

#include <format>
#include <ranges>
#include <string_view>
#include <vector>

#include <lambda/core/parse_error.hpp>

namespace ld {

constexpr bool is_in(std::string_view set, char c) {
    return std::ranges::find(set, c) != set.end();
};

constexpr bool is_alpha_num(char c) {
    return is_in("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", c);
}

constexpr bool is_space(char c) {
    return is_in(" \t\r\n\f\v", c);
}

constexpr bool is_identifier_char(char c) {
    /* BEFORE MODIFYING!
    Remember to reflect any changes in shell::DeclareCommand help text.
    */
    static constexpr std::string_view ExtraIdentifierCharSet = "_-*$+/~@";
    return is_alpha_num(c) or is_in(ExtraIdentifierCharSet, c);
}

struct Token {
    struct Kind {
        enum _kind {
            Lambda,
            Dot,
            LeftParenthesis,
            RightParenthesis,
            Identifier,
            Whitespace,
            Invalid
        };

        _kind kind;

        constexpr Kind(_kind k) noexcept : kind(k) { }
        constexpr Kind(char c)  noexcept : kind(from_char(c)) { }

        constexpr operator _kind() const noexcept { return kind; }

        template<typename... Ts> requires (std::same_as<Ts, _kind> and ...)
        constexpr bool is_one_of(Ts ...kinds) noexcept {
            return ((kind == kinds) or ...);
        }

        static constexpr Token::Kind::_kind from_char(char c) noexcept {
            using enum _kind;

            if (c == '\\')             { return Lambda; }
            if (c == '.')              { return Dot; }
            if (c == '(')              { return LeftParenthesis; }
            if (c == ')')              { return RightParenthesis; }
            if (is_identifier_char(c)) { return Identifier; }
            if (is_space(c))           { return Whitespace; }

            return Invalid;
        }
    };

    Kind kind;
    std::string_view text;

    friend constexpr bool operator==(const Token& left, const Token& right) noexcept = default;
};

using ETokenKind = Token::Kind::_kind; // provided for using enum purposes (using enum Token::Kind does not work)

struct TokenizedSourceView {
    std::string_view text;
    std::vector<Token> tokens;

    friend constexpr bool operator==(const TokenizedSourceView& left, const TokenizedSourceView& right) noexcept = default;
};

constexpr TokenizedSourceView tokenize(std::string_view source) {
    namespace stdv = std::views;
    using enum ETokenKind;

    if (source.empty()) return {};

    std::vector<Token> tokens;
    // all tokens are single characters, except for whitespace and identifiers.
    // source size is a good approximation of the amount of tokens.
    tokens.reserve(source.size());

    auto last_token_kind = Token::Kind(*source.begin());
    if (last_token_kind == Invalid) {
        throw parse_error("invalid token at char 0", *source.begin());
    }

    std::size_t last_token_start = 0;
    for (auto [pos, c] : source | stdv::enumerate | stdv::drop(1)) {
        auto kind = Token::Kind(c);
        if (kind == Invalid) {
            throw parse_error(std::format("invalid token at char {}", pos), c);
        }

        if (kind.is_one_of(Whitespace, Identifier) and kind == last_token_kind) {
            // aggregate consecutive whitespace tokens and consecutive identifier tokens by ignoring them
            continue;
        } else {
            // tokens are only pushed on the list when the start of a new token is encountered, as
            // this makes it simpler to associate the correct source info to multi-character tokens. 
            auto last_token_source = std::string_view(source.data() + last_token_start, source.data() + pos);
            tokens.push_back({ last_token_kind, last_token_source});

            last_token_kind = kind;
            last_token_start = pos;
        }
    }

    auto last_token_source = std::string_view(source.begin() + last_token_start, source.end());
    tokens.push_back({ last_token_kind, last_token_source});

    return {
        .text = source,
        .tokens = std::move(tokens)
    };
}

constexpr std::string_view to_string(ETokenKind token_kind) {
    using enum ETokenKind;

    switch(token_kind) {
    case Lambda:           return "Lambda";
    case Dot:              return "Dot";
    case LeftParenthesis:  return "LeftParenthesis";
    case RightParenthesis: return "RightParenthesis";
    case Identifier:       return "Identifier";
    case Whitespace:       return "Whitespace";
    case Invalid:          return "Invalid";
    default:               return "Unknown";
    }
}

} // namespace ld

#endif//LAMBDA_TOKENIZE_HPP