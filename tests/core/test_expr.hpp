#ifndef TESTS_CORE_TEST_EXPR_HPP
#define TESTS_CORE_TEST_EXPR_HPP

#include <initializer_list>
#include <vector>

#include <lambda/core/ast.hpp>
#include <lambda/core/token.hpp>
#include <lambda/core/parse.hpp>

namespace ld::expr {

using ETk = ETokenKind;

// token info
struct Tk {
    ETokenKind kind;
    int size = 1;
};

struct TkList {
    std::vector<Tk> list;

    TkList(std::initializer_list<Tk> list) : list(list) {}
};

// syntax tree info
struct St {
    SyntaxNode::LexicalExpression lex;
    int off;
    int count = 1;
    std::vector<St> children;

    SyntaxTree to_syntax_tree(std::string_view source) const {
        SyntaxTree result;
        result.emplace_node(result.root(), lex, source.substr(off, count));
        for (const auto& child : children) {
            result.adopt_subtree(result.root(), child.to_syntax_tree(source));
        }

        return result;
    }
};

struct TestAST {
    std::string_view source;
    SyntaxTree tree;
};

inline bool operator==(const TestAST& left, const AST& right) {
    return left.source == right.source() and left.tree == right.tree();
}

inline bool operator==(const AST& left, const TestAST& right) {
    return right == left;
}

struct TestExpr {
    std::string_view text;
    TokenizedSourceView tokenized_source;
    TestAST ast;

    TestExpr(std::string_view txt, const TkList& tks) {
        std::vector<Token> tokens;
        tokens.reserve(tks.list.size());
        std::size_t offset = 0;
        for (const auto& tk : tks.list) {
            auto off = std::exchange(offset, offset + tk.size);
            tokens.emplace_back(tk.kind, txt.substr(off, tk.size));
        }
        
        text = txt;

        tokenized_source = {
            .text = text,
            .tokens = std::move(tokens)
        };
    }

    TestExpr(std::string_view txt, const TkList& tks, const St& tree)
        : TestExpr(txt, tks)
    {
        ast.source = txt;
        ast.tree = tree.to_syntax_tree(ast.source);
    }
};

} // namespace ld

#endif//TESTS_CORE_TEST_EXPR_HPP