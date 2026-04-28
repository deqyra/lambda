#include <lambda/core/ast.hpp>

namespace ld {

AST::AST(SyntaxTree tree)
{
    if (not tree.empty()) {
        _tree = std::move(tree);
        _source = minimal_source_from_syntax_subtree(_tree.root());
        _rebind();
    }
}

AST::AST(const AST& other) {
    *this = other;
}

AST::AST(AST&& other) noexcept {
    *this = std::move(other);
}

AST& AST::operator=(const AST& other) {
    _source = other._source;
    _tree = other._tree;
    _rebind();

    return *this;
}

AST& AST::operator=(AST&& other) noexcept {
    _source = std::move(other._source);
    _tree = std::move(other._tree);
    _rebind();

    return *this;
}

std::string_view AST::source() const {
    return _source;
}

const SyntaxTree& AST::tree() const {
    return _tree;
}

void AST::_rebind() {
    if (not _tree.empty()) {
        rebind_text_from_minimal_source(_source, _tree.root());
    }
}

AST parse_full_expression(const TokenizedSourceView& source) {
    using enum ETokenKind;

    ParseContext ctx(source);
    ctx.exhaust_whitespace();

    if (ctx.empty()) {
        return {};
    }

    SyntaxTree syntax_tree = parse_expression(ctx);

    ctx.exhaust_whitespace();
    if (not ctx.empty()) {
        throw parse_error("unexpected tokens past end of full expression at char {}", ctx.text_pos());
    }

    return AST(std::move(syntax_tree));
}

}