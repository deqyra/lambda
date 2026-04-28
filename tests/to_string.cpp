#include <tests/to_string.hpp>

#include <cpptools/utility/overloaded_visitor.hpp>

#include <format>

namespace ld {

std::string to_string(const Token& tok) {
    return std::format("{}: {}", to_string(tok.kind), tok.text);
}

std::string_view to_string(const SyntaxNode::LexicalExpression& expr) {
    auto to_str = tools::overloaded_visitor{
        [](const Identifier&) -> std::string_view {
            return "Identifier";
        },
        [](const Variable&) -> std::string_view {
            return "Variable";
        },
        [](const Application&) -> std::string_view {
            return "Application";
        },
        [](const Abstraction&) -> std::string_view {
            return "Abstraction";
        }
    };

    return std::visit(to_str, expr);
}

std::string to_string(const SyntaxNode& node) {
    return std::format("{}: {}", to_string(node.expr), node.text);
}

std::string to_string(SyntaxConstNodeHandle root, int depth) {
    auto result = std::string(depth * 2, ' ') + to_string(*root) + '\n';
    for (const auto& child : root.children()) {
        result += to_string(child, depth + 1);
    }
    if (depth == 0) {
        result.pop_back(); // remove extraneous '\n'
    }
    return result;
}

std::string to_string(const SyntaxTree& tree) {
    if (tree.empty()) {
        return "";
    }

    return to_string(tree.root());
}

std::string to_string(const AST& ast) {
    return to_string(ast.tree());
}

}