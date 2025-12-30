#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "ir.h"
#include <vector>

class Optimizer {
public:
    // Apply small, local optimizations to the IR and return a new IR list.
    std::vector<IRInstruction> optimize(const std::vector<IRInstruction>& ir);
};

#endif
