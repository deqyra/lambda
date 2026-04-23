#ifndef TESTS_OSTREAM_DUMP_HPP
#define TESTS_OSTREAM_DUMP_HPP

#include <lambda/core/token.hpp>
#include <lambda/core/parse.hpp>

namespace ld {

std::ostream& operator<<(std::ostream& out, const Token& tok);
std::ostream& operator<<(std::ostream& out, const SyntaxNode::LexicalExpression& expr);
std::ostream& operator<<(std::ostream& out, const SyntaxNode& node);
std::ostream& operator<<(std::ostream& out, const tools::tree<SyntaxNode>& tree);
std::ostream& operator<<(std::ostream& out, tools::tree<SyntaxNode>::const_node_handle_t root);
std::ostream& operator<<(std::ostream& out, const AST& ast);

} // namespace ld

#endif//TESTS_OSTREAM_DUMP_HPP