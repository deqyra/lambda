#ifndef TESTS_CORE_TEST_EXPR_HPP
#define TESTS_CORE_TEST_EXPR_HPP

#include <initializer_list>
#include <vector>

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

    tools::tree<SyntaxNode> to_syntax_tree(std::string_view source) const {
        tools::tree<SyntaxNode> result;
        result.emplace_node(result.root(), lex, source.substr(off, count));
        for (const auto& child : children) {
            result.adopt_subtree(result.root(), child.to_syntax_tree(source));
        }

        return result;
    }
};

struct TestExpr {
    std::string_view text;
    TokenizedSourceView tokenized_source;
    AST ast;

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
        ast.source = std::string(txt);
        ast.tree = tree.to_syntax_tree(ast.source);
    }
};

} // namespace ld

#endif//TESTS_CORE_TEST_EXPR_HPP