#ifndef CODEGEN_H
#define CODEGEN_H

#include "ir.h"
#include <vector>
#include <string>

class CodeGenerator {
public:
    // Generate toy assembly text lines from IR and return them
    std::vector<std::string> generateAssembly(const std::vector<IRInstruction>& ir);
};

#endif
