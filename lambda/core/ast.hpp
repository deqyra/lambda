#ifndef LAMBDA_CORE_AST_HPP
#define LAMBDA_CORE_AST_HPP

#include <lambda/core/parse.hpp>
#include <lambda/core/token.hpp>

namespace ld
{

// abstract syntax tree
class AST {
    std::string _source;
    SyntaxTree _tree;

    void _rebind();

public:
    AST() = default;
    AST(SyntaxTree tree);
    AST(const AST& other);
    AST(AST&& other) noexcept;
    AST& operator=(const AST& other);
    AST& operator=(AST&& other) noexcept;

    std::string_view source() const;
    const SyntaxTree& tree() const;

    friend bool operator==(const AST& left, const AST& right) = default;
};

AST parse_full_expression(const TokenizedSourceView& source);

}

#endif//LAMBDA_CORE_AST_HPP
