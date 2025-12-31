#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "ir.h"
#include <vector>

// This class is responsible for optimizing the intermediate representation (IR) of the code.
class Optimizer {
public:
    // Apply small, local optimizations to the IR and return a new IR list.
    std::vector<IRInstruction> optimize(const std::vector<IRInstruction>& ir);
};

#endif
