#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>

#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "ir.h"
#include "interpreter.h"

#include "optimizer.h"
#include "codegen.h"

namespace fs = std::filesystem;

int runCompilerOnFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string code = buffer.str();

    try {
        Lexer lexer(code);
        Parser parser(lexer);
        std::vector<ASTNode*> ast = parser.parse();

        // Semantic phase
        std::cout << "=== Semantic Analysis ===\n";
        SemanticAnalyzer semantic;
        try {
            semantic.analyze(ast);
            std::cout << "OK\n\n";
        } catch (const std::exception& e) {
            std::cerr << "[SEMANTIC ERROR] " << e.what() << "\n";
            return 1;
        }

        // IR generation
        std::cout << "=== Generating IR ===\n";
        IRGenerator irgen;
        std::vector<IRInstruction> ir = irgen.generate(ast);

        auto printIR = [](const IRInstruction& i) {
            if (!i.arg2.empty())
                std::cout << i.op << " " << i.arg1 << " " << i.arg2 << " -> " << i.result << "\n";
            else if (!i.arg1.empty() && !i.result.empty())
                std::cout << i.op << " " << i.arg1 << " -> " << i.result << "\n";
            else if (!i.arg1.empty())
                std::cout << i.op << " " << i.arg1 << "\n";
            else
                std::cout << i.op << "\n";
        };

        for (auto& i : ir) printIR(i);
        std::cout << "\n";

        // Optimize IR
        std::cout << "=== Optimizing IR ===\n";
        Optimizer opt;
        std::vector<IRInstruction> optimizedIR = opt.optimize(ir);
        for (auto& i : optimizedIR) printIR(i);
        std::cout << "\n";

        // Code generation
        std::cout << "=== Code Generation ===\n";
        CodeGenerator codegen;
        auto asmCode = codegen.generateAssembly(optimizedIR);
        for (auto &line : asmCode) std::cout << line << "\n";
        std::cout << "\n";

        // Run / Interpret
        std::cout << "=== Running Program ===\n";
        Interpreter interpreter;
        interpreter.execute(ast);

    } catch (const std::exception& e) {
        std::cerr << "Error while running " << filename << ": " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::string testFolder = "tests";
        if (!fs::exists(testFolder)) {
            std::cerr << "No tests folder found.\n";
            return 1;
        }

        for (auto& entry : fs::directory_iterator(testFolder)) {
            if (entry.path().extension() == ".txt") {
                std::cout << "\n=== Running " << entry.path().string() << " ===\n";
                runCompilerOnFile(entry.path().string());
                std::cout << "-------------------------------------\n";
            }
        }
    } else {
        runCompilerOnFile(argv[1]);
    }

    return 0;
}
