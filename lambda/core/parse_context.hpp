#ifndef LAMBDA_PARSE_CONTEXT_HPP
#define LAMBDA_PARSE_CONTEXT_HPP

#include <algorithm>
#include <cstddef>

#include <lambda/core/parse_error.hpp>
#include <lambda/core/token.hpp>

namespace ld {

class ParseContext {
    const TokenizedSourceView& _source;
    std::size_t _text_pos;
    std::size_t _token_pos;

public:
    constexpr ParseContext(const TokenizedSourceView& source) noexcept
        : _source(source)
        , _text_pos(0)
        , _token_pos(0)
    {

    }

    constexpr ParseContext(const ParseContext& other) noexcept = default;
    constexpr ParseContext(ParseContext&& other) noexcept = default;
    ParseContext& operator=(const ParseContext& other) = delete;
    ParseContext& operator=(ParseContext&& other) = delete;

    constexpr bool empty() noexcept {
        return _token_pos >= _source.tokens.size();
    }

    constexpr const Token& peek(std::size_t pos = 0) {
        auto i = _token_pos + pos;
        if (i >= _source.tokens.size()) {
            throw parse_error("parse context exhausted, cannot peek token");
        }
        return _source.tokens[i];
    }

    constexpr void advance(std::size_t pos = 1) {
        _token_pos = std::min(_token_pos + pos, _source.tokens.size());

        if (not empty()) {
            _text_pos = peek().text.data() - _source.text.data();
        } else {
            _text_pos = _source.text.size();
        }
    }

    constexpr const Token& next() {
        const auto& result = peek();
        advance();
        return result;
    }

    constexpr void exhaust_whitespace() {
        using enum ETokenKind;
        while (not empty() and peek().kind == Whitespace) advance();
    }

    constexpr const char* data() noexcept {
        return _source.text.data() + _text_pos;
    }

    constexpr std::size_t text_pos() noexcept {
        return _text_pos;
    }

    constexpr std::size_t token_pos() noexcept {
        return _token_pos;
    }

    constexpr const Token& expect(ETokenKind token_kind) {
        const auto& result = peek();
        if (result.kind != token_kind) {
            throw parse_error(std::format("expected {}", to_string(token_kind)), result.text);
        }

        return result;
    }

    constexpr const Token& expect_and_consume(ETokenKind token_kind) {
        const auto& result = expect(token_kind);
        advance();
        return result;
    }
};

} // namespace ld

#endif//LAMBDA_PARSE_CONTEXT_HPP