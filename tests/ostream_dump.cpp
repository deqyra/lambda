#include <tests/ostream_dump.hpp>
#include <tests/to_string.hpp>

#include <cpptools/utility/overloaded_visitor.hpp>

#include <lambda/core/parse.hpp>

namespace ld {

std::ostream& operator<<(std::ostream& out, const Token& tok) {
    return (out << to_string(tok));
}

std::ostream& operator<<(std::ostream& out, const SyntaxNode::LexicalExpression& expr) {
    return (out << to_string(expr));
}

std::ostream& operator<<(std::ostream& out, const SyntaxNode& node) {
    return (out << to_string(node));
}

std::ostream& operator<<(std::ostream& out, tools::tree<SyntaxNode>::const_node_handle_t root) {
    return (out << to_string(root));
}

std::ostream& operator<<(std::ostream& out, const tools::tree<SyntaxNode>& tree) {
    return (out << to_string(tree));
}

std::ostream& operator<<(std::ostream& out, const AST& ast) {
    return (out << to_string(ast));
}

} // namespace ld
