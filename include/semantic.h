#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "parser.h"
#include <unordered_map>
#include <string>
#include <vector>

class SemanticAnalyzer {
private:
    std::unordered_map<std::string, bool> declared;

public:
    void analyze(const std::vector<ASTNode*>& ast);
    void analyzeNode(ASTNode* node);
};

#endif
