#include "ir.h"
#include "parser.h"
#include <vector>
#include <string>
#include <sstream> //
#include <functional> // <--- added to fix std::function compile error

// Helper to create temporary names
static std::string makeTemp(int n) {
    return std::string("t") + std::to_string(n);
}

//as input AST and IR as output
std::vector<IRInstruction> IRGenerator::generate(const std::vector<ASTNode*>& nodes) {
    std::vector<IRInstruction> ir;
    int tmpCount = 1;

    // forward declare lambda
    std::function<std::string(ASTNode*)> genExpr;

    genExpr = [&](ASTNode* node) -> std::string {
        if (!node) return "";

        if (node->type == "number") {
            std::string t = makeTemp(tmpCount++);
            ir.push_back(IRInstruction{"MOV", node->value, "", t});
            return t;
        }

        if (node->type == "string") {
            // store string literal including quotes so it is clear in IR
            std::string literal = "\"" + node->value + "\"";
            std::string t = makeTemp(tmpCount++);
            ir.push_back(IRInstruction{"MOV", literal, "", t});
            return t;
        }

        if (node->type == "variable") {
            // read variable into a temp
            std::string t = makeTemp(tmpCount++);
            ir.push_back(IRInstruction{"LOAD", node->name, "", t});
            return t;
        }

        if (node->type == "binop") {
            std::string L = genExpr(node->left);
            std::string R = genExpr(node->right);
            std::string t = makeTemp(tmpCount++);

            std::string op = node->op;
            // normalize operator tokens to IR ops
            if (op == "+") op = "ADD";
            else if (op == "-") op = "SUB";
            else if (op == "*") op = "MUL";
            else if (op == "/") op = "DIV";

            ir.push_back(IRInstruction{op, L, R, t});

            // if we see DIV with literal 0 on right, also add an ERROR node to document it
            if ((op == "DIV") && node->right && node->right->type == "number" && node->right->value == "0") {
                std::ostringstream msg;
                msg << "; ERROR: division by zero at line " << node->line;
                ir.push_back(IRInstruction{msg.str(), "", "", ""});
            }

            return t;
        }

        // fallback
        return std::string();
    };

    for (ASTNode* stmt : nodes) {
        if (!stmt) continue;

        if (stmt->type == "assign") {
            // evaluate RHS into a temp (or variable result)
            std::string rhs = genExpr(stmt->left);
            // store temp into the variable
            ir.push_back(IRInstruction{"STORE", rhs, "", stmt->name});
            continue;
        }

        if (stmt->type == "cout") {
            // evaluate expression and print either variable name or temp
            std::string rhs = genExpr(stmt->left);
            if (stmt->left && stmt->left->type == "variable") {
                // print variable directly (LOAD would be emitted elsewhere)
                ir.push_back(IRInstruction{"PRINT", stmt->left->name, "", ""});
            } else {
                ir.push_back(IRInstruction{"PRINT", rhs, "", ""});
            }
            continue;
        }

        if (stmt->type == "cin") {
            // read into variable (represent as a special STORE from input)
            ir.push_back(IRInstruction{"READ", "", "", stmt->name});
            continue;
        }

        std::ostringstream note;
        note << "; UNHANDLED_STMT type=" << stmt->type << " line=" << stmt->line;
        ir.push_back(IRInstruction{note.str(), "", "", ""});
    }

    return ir;
}
