#ifndef CODEGEN_H
#define CODEGEN_H

#include "ir.h"
#include <vector>
#include <string>

// This class is responsible for generating assembly code from IR.
class CodeGenerator {
public:
    // Generate toy assembly text lines from IR and return them
    std::vector<std::string> generateAssembly(const std::vector<IRInstruction>& ir);
};

#endif
