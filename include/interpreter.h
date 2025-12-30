#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "parser.h"
#include <unordered_map>
#include <string>
#include <vector>

class Interpreter {
private:
    std::unordered_map<std::string, std::string> variables;

    // Evaluate node and return its string representation (numbers converted to strings)
    std::string eval(ASTNode* node);

    // Helper to check if string represents an integer and parse it
    bool tryParseInt(const std::string& s, int& out);

public:
    void execute(const std::vector<ASTNode*>& nodes);
};

#endif
