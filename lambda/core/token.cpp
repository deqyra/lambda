#include <lambda/core/token.hpp>

#include <format>
#include <ranges>

namespace ld {

TokenizedSourceView tokenize(std::string_view source){
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
        }

        // getting past this point means a new token was encountered

        // tokens are only pushed on the list when the start of a new token is encountered, as
        // this makes it simpler to associate the correct source info to multi-character tokens. 
        auto last_token_source = std::string_view(source.data() + last_token_start, source.data() + pos);
        tokens.push_back({ last_token_kind, last_token_source});

        // mark start of new token
        last_token_kind = kind;
        last_token_start = pos;
    }

    // push last token
    auto last_token_source = std::string_view(source.begin() + last_token_start, source.end());
    tokens.push_back({ last_token_kind, last_token_source});

    return {
        .text = source,
        .tokens = std::move(tokens)
    };
}

}