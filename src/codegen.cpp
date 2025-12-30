#include "codegen.h"
#include "ir.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <cctype>

static bool isNumber(const std::string& s) {
    if (s.empty()) return false;
    size_t i = 0;
    if (s[0] == '+' || s[0] == '-') i = 1;
    for (; i < s.size(); ++i) if (!std::isdigit(static_cast<unsigned char>(s[i]))) return false;
    return true;
}
static bool isQuotedString(const std::string& s) {
    return s.size() >= 2 && s.front() == '"' && s.back() == '"';
}

std::vector<std::string> CodeGenerator::generateAssembly(const std::vector<IRInstruction>& ir) {
    std::vector<std::string> out;
    std::unordered_map<std::string,std::string> reg;
    int rc = 1;

    auto alloc = [&](const std::string& name) {
        auto it = reg.find(name);
        if (it != reg.end()) return it->second;
        std::string r = "R" + std::to_string(rc++);
        reg[name] = r;
        return r;
    };

    for (const auto &ins : ir) {
        if (!ins.op.empty() && ins.op[0] == ';') {
            out.push_back(ins.op);
            continue;
        }

        std::string op = ins.op;

        if (op == "MOV") {
            if (isNumber(ins.arg1) || isQuotedString(ins.arg1)) {
                std::string dest = alloc(ins.result);
                out.push_back(std::string("LOAD ") + ins.arg1 + ", " + dest);
            } else {
                std::string src = alloc(ins.arg1);
                std::string dest = alloc(ins.result);
                out.push_back(std::string("MOV ") + src + ", " + dest);
            }
            continue;
        }

        if (op == "LOAD") {
            std::string dst = alloc(ins.result);
            out.push_back(std::string("LOAD ") + ins.arg1 + ", " + dst);
            continue;
        }

        if (op == "STORE") {
            std::string src = alloc(ins.arg1);
            out.push_back(std::string("STORE ") + src + ", " + ins.result);
            continue;
        }

        if (op == "PRINT") {
            if (isNumber(ins.arg1) || isQuotedString(ins.arg1)) {
                out.push_back(std::string("PRINT ") + ins.arg1);
            } else {
                std::string r = alloc(ins.arg1);
                out.push_back(std::string("PRINT ") + r);
            }
            continue;
        }

        if (op == "READ") {
            // READ -> var (represent as reading into register then storing)
            std::string dst = alloc(ins.result);
            out.push_back(std::string("READ -> ") + dst);
            out.push_back(std::string("STORE ") + dst + ", " + ins.result);
            continue;
        }

        // arithmetic ops
        if (op == "ADD" || op == "SUB" || op == "MUL" || op == "DIV") {
            std::string a = ins.arg1;
            std::string b = ins.arg2;
            std::string ra, rb;

            // ensure left operand in register
            if (isNumber(a) || isQuotedString(a)) {
                ra = alloc("<const#" + a + ">");
                out.push_back(std::string("LOAD ") + a + ", " + ra);
            } else {
                ra = alloc(a);
            }

            // ensure right operand in register
            if (isNumber(b) || isQuotedString(b)) {
                rb = alloc("<const#" + b + ">");
                out.push_back(std::string("LOAD ") + b + ", " + rb);
            } else {
                rb = alloc(b);
            }

            std::string rd = alloc(ins.result);
            std::string opcode = (op == "ADD") ? "ADD" : (op == "SUB") ? "SUB" : (op == "MUL") ? "MUL" : "DIV";
            out.push_back(opcode + " " + ra + ", " + rb + ", " + rd);
            continue;
        }

        // If it's a comment-like ERROR entry, propagate as PRINT-able text
        if (!op.empty() && op.rfind("; ERROR", 0) == 0) {
            out.push_back(op);
            continue;
        }

        // Unhandled IR: emit as comment so it's visible
        out.push_back(std::string("; UNHANDLED IR: ") + op + " " + ins.arg1 + " " + ins.arg2 + " -> " + ins.result);
    }

    if (out.empty()) out.push_back("; <no assembly generated>");
    return out;
}
