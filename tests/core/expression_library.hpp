#include <lambda/core/token.hpp>
#include <lambda/core/parse.hpp>

namespace ld::expr {

namespace text {
    inline constexpr std::string_view One = R"(\f.\x.(f x))";
}

namespace tokens {
    inline const TokenizedSource One = {
        .text = text::One,
        .tokens = {
            { Token::Kind::Lambda,           text::One.substr( 0, 1) },
            { Token::Kind::Identifier,       text::One.substr( 1, 1) },
            { Token::Kind::Dot,              text::One.substr( 2, 1) },
            { Token::Kind::Lambda,           text::One.substr( 3, 1) },
            { Token::Kind::Identifier,       text::One.substr( 4, 1) },
            { Token::Kind::Dot,              text::One.substr( 5, 1) },
            { Token::Kind::LeftParenthesis,  text::One.substr( 6, 1) },
            { Token::Kind::Identifier,       text::One.substr( 7, 1) },
            { Token::Kind::Whitespace,       text::One.substr( 8, 1) },
            { Token::Kind::Identifier,       text::One.substr( 9, 1) },
            { Token::Kind::RightParenthesis, text::One.substr(10, 1) }
        }
    };
}

namespace ast {
    inline const AST One = {
        .source = text::One,
        .tree = {{
            { Abstraction{}, text::One.substr(0, 11) }, {
                {{ Variable{}, text::One.substr(1, 1) }, {}},
                {{ Abstraction{}, text::One.substr(3, 8) }, {
                    {{ Variable{}, text::One.substr(4, 1) }, {}},
                    {{ Application{}, text::One.substr(6, 5) }, {
                        {{ Identifier{}, text::One.substr(7, 1) }, {}},
                        {{ Identifier{}, text::One.substr(9, 1) }, {}}
                    }},
                }},
            }
        }}
    };
}

struct TestExpr {
    std::string_view text;
    TokenizedSource tokenizedSource;
    AST ast;
};

const TestExpr One = {
    .text = text::One,
    .tokenizedSource = tokens::One,
    .ast = ast::One
};

}