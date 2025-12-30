#ifndef IR_H
#define IR_H

#include "parser.h"
#include <string>
#include <vector>

struct IRInstruction {
    std::string op;
    std::string arg1;
    std::string arg2;
    std::string result;
};

class IRGenerator {
public:
    std::vector<IRInstruction> generate(const std::vector<ASTNode*>& ast);
    void genNode(ASTNode* node, std::vector<IRInstruction>& ir);
};

#endif
