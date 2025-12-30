#include "interpreter.h"
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cctype>
#include <string>

bool Interpreter::tryParseInt(const std::string& s, int& out) {
    try {
        size_t idx = 0;
        long v = std::stol(s, &idx, 10);
        if (idx != s.size()) return false;
        out = static_cast<int>(v);
        return true;
    } catch (...) {
        return false;
    }
}

std::string Interpreter::eval(ASTNode* node) {
    if (!node) return "";

    // literals
    if (node->type == "number") return node->value;
    if (node->type == "string") return node->value;

    if (node->type == "variable") {
        if (variables.find(node->name) == variables.end())
            throw std::runtime_error("Runtime error: Undefined variable '" + node->name + "' at line " + std::to_string(node->line));
        return variables[node->name];
    }

    if (node->type == "binop") {
        std::string left = eval(node->left);
        std::string right = eval(node->right);
        std::string op = node->op;

        int li, ri;
        bool leftIsNum = tryParseInt(left, li);
        bool rightIsNum = tryParseInt(right, ri);

        if (op == "+") {
            // if both numeric => arithmetic; else concatenate (number converted to string if needed)
            if (leftIsNum && rightIsNum) {
                return std::to_string(li + ri);
            } else {
                return left + right;
            }
        }
        if (op == "-") {
            if (!leftIsNum || !rightIsNum) throw std::runtime_error("Runtime error: Cannot subtract non-numeric values at line " + std::to_string(node->line));
            return std::to_string(li - ri);
        }
        if (op == "*") {
            if (!leftIsNum || !rightIsNum) throw std::runtime_error("Runtime error: Cannot multiply non-numeric values at line " + std::to_string(node->line));
            return std::to_string(li * ri);
        }
        if (op == "/") {
            if (!leftIsNum || !rightIsNum) throw std::runtime_error("Runtime error: Cannot divide non-numeric values at line " + std::to_string(node->line));
            if (ri == 0) throw std::runtime_error("Runtime error: Division by zero at line " + std::to_string(node->line));
            return std::to_string(li / ri);
        }

        throw std::runtime_error("Runtime error: Unknown operator '" + op + "' at line " + std::to_string(node->line));
    }

    if (node->type == "assign") {
        std::string val;
        try {
            val = eval(node->left);
        } catch (const std::exception& e) {
            // Add line context if the inner exception doesn't have it
            throw;
        }
        variables[node->name] = val;
        return val;
    }

    if (node->type == "cin") {
        std::string input;
        if (!std::getline(std::cin, input)) input = "";
        variables[node->name] = input;
        return input;
    }

    if (node->type == "cout") {
        std::string val;
        try {
            val = eval(node->left);
        } catch (const std::exception& e) {
            throw;
        }
        std::cout << val << std::endl;
        return val;
    }

    return "";
}

void Interpreter::execute(const std::vector<ASTNode*>& nodes) {
    for (auto node : nodes) {
        try {
            eval(node);
        } catch (const std::exception& e) {
            // Print error message and continue with next node
            std::cerr << e.what() << std::endl;
        }
    }
}
