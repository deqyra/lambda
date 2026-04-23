#ifndef TESTS_TO_STRING_HPP
#define TESTS_TO_STRING_HPP

#include <string>
#include <string_view>

#include <cpptools/utility/overloaded_visitor.hpp>

#include <lambda/core/parse.hpp>

namespace ld {
    
std::string to_string(const Token& tok);
std::string_view to_string(const SyntaxNode::LexicalExpression& expr);
std::string to_string(const SyntaxNode& node);
std::string to_string(tools::tree<SyntaxNode>::const_node_handle_t root, int depth = 0);
std::string to_string(const tools::tree<SyntaxNode>& tree);
std::string to_string(const AST& ast);

} // namespace ld

#endif//TESTS_TO_STRING_HPP