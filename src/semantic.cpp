#include "semantic.h"
#include "parser.h"
#include <vector>
#include <stdexcept>

// Minimal semantic analyzer implementation to satisfy the linker.
// Expand with real checks as needed.
void SemanticAnalyzer::analyze(const std::vector<ASTNode*>& nodes) {
    for (auto n : nodes) {
        analyzeNode(n);
    }
}

void SemanticAnalyzer::analyzeNode(ASTNode* node) {
    if (!node) return;

    // Assignment introduces variable (variable name is stored in `name`)
    if (node->type == "assign") {
        declared[node->name] = true;
        analyzeNode(node->left);
        return;
    }

    // Variable usage must be declared
    if (node->type == "variable") {
        if (!declared.count(node->name)) {
            throw std::runtime_error("Use of undeclared variable: " + node->name + " at line " + std::to_string(node->line));
        }
        return;
    }
    // binary operation checks
    if (node->type == "binop") {
        analyzeNode(node->left);
        analyzeNode(node->right);
        return;
    }

    if (node->type == "cout" || node->type == "cin") {
        if (node->type == "cin" && !node->name.empty()) {
            declared[node->name] = true;
        }
        analyzeNode(node->left);// output statement are checked to ensure the expression is semantically valid
        return;
    }
}
