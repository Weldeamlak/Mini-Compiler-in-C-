#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "parser.h"
#include <unordered_map>//used as symbol table
#include <string>
#include <vector>

class SemanticAnalyzer {
private:
    std::unordered_map<std::string, bool> declared;
    // used to track declared variables

public:
    void analyze(const std::vector<ASTNode*>& ast);
    void analyzeNode(ASTNode* node);// check variable declarations node by node
};

#endif
