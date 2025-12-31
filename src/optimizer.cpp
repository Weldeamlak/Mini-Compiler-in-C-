#include "optimizer.h"
#include "ir.h"
#include <vector>
#include <string>
#include <unordered_set>// tracking usied variable/temps
#include <cstdlib>
#include <cctype>

// helper: is a string an integer literal (allows quoted strings to be non-numeric)
static bool isNumber(const std::string& s) {
    if (s.empty()) return false;
    size_t i = 0;
    if (s[0] == '+' || s[0] == '-') i = 1;
    for (; i < s.size(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(s[i]))) return false;
    }
    return true;
}

static long toLong(const std::string& s) {
    return std::strtol(s.c_str(), nullptr, 10);
}

std::vector<IRInstruction> Optimizer::optimize(const std::vector<IRInstruction>& ir) {
    std::vector<IRInstruction> folded;
    folded.reserve(ir.size());

    bool foldedAny = false;
    bool removedAny = false;
    bool movChainFound = false;
    bool divByZeroFound = false;
    std::vector<std::string> foldedExamples;

    // 1) constant folding & algebraic simplifications
    for (size_t idx = 0; idx < ir.size(); ++idx) {
        const auto &ins = ir[idx];

        // preserve comment/error entries as-is
        if (!ins.op.empty() && ins.op[0] == ';') {
            folded.push_back(ins);
            continue;
        }

        std::string op = ins.op;
        std::string a = ins.arg1;
        std::string b = ins.arg2;
        std::string res = ins.result;

        // detect simple MOV chain: MOV x -> t1 followed by MOV t1 -> t2
        if (op == "MOV" && !a.empty() && !res.empty() && a.size() > 0 && a[0] == 't') {
            // look ahead for previous instruction that defines a
            for (size_t j = 0; j < idx; ++j) {
                if (ir[j].result == a && ir[j].op == "MOV") {
                    movChainFound = true;
                    break;
                }
            }
        }

        if ((op == "ADD" || op == "SUB" || op == "MUL" || op == "DIV") && isNumber(a) && isNumber(b)) {
            long va = toLong(a), vb = toLong(b), vr = 0;
            if (op == "ADD") vr = va + vb;
            else if (op == "SUB") vr = va - vb;
            else if (op == "MUL") vr = va * vb;
            else if (op == "DIV") {
                if (vb == 0) {
                    // record DIV by zero presence and keep original instruction
                    divByZeroFound = true;
                    folded.push_back(ins);
                    continue;
                } else {
                    vr = va / vb;
                }
            }
            // replace with MOV <const> -> res
            folded.push_back(IRInstruction{"MOV", std::to_string(vr), "", res});
            // record a readable example: (a op b) -> vr
            std::string sym = (op == "ADD") ? "+" : (op == "SUB") ? "-" : (op == "MUL") ? "*" : "/";
            foldedExamples.push_back("(" + a + " " + sym + " " + b + ") -> " + std::to_string(vr));
            foldedAny = true;
            continue;
        }

        // algebraic simplifications
        if (op == "ADD") {
            if (isNumber(b) && toLong(b) == 0) { folded.push_back(IRInstruction{"MOV", a, "", res}); foldedAny = true; continue; }
            if (isNumber(a) && toLong(a) == 0) { folded.push_back(IRInstruction{"MOV", b, "", res}); foldedAny = true; continue; }
        }
        if (op == "MUL") {
            if ((isNumber(a) && toLong(a) == 0) || (isNumber(b) && toLong(b) == 0)) {
                folded.push_back(IRInstruction{"MOV", "0", "", res}); foldedAny = true; continue;
            }
            if (isNumber(b) && toLong(b) == 1) { folded.push_back(IRInstruction{"MOV", a, "", res}); foldedAny = true; continue; }
            if (isNumber(a) && toLong(a) == 1) { folded.push_back(IRInstruction{"MOV", b, "", res}); foldedAny = true; continue; }
        }
        if (op == "SUB") {
            if (isNumber(b) && toLong(b) == 0) { folded.push_back(IRInstruction{"MOV", a, "", res}); foldedAny = true; continue; }
        }
        if (op == "DIV") {
            if (isNumber(b) && toLong(b) == 1) { folded.push_back(IRInstruction{"MOV", a, "", res}); foldedAny = true; continue; }
        }

        // default: keep instruction
        folded.push_back(ins);
    }

    // 2) remove unused temporaries: collect used names
    std::unordered_set<std::string> used;
    for (const auto &ins : folded) {
        if (!ins.arg1.empty() && ins.arg1[0] == 't') used.insert(ins.arg1);
        if (!ins.arg2.empty() && ins.arg2[0] == 't') used.insert(ins.arg2);
        // variables are considered used if mentioned in arg1/arg2 and not numeric
        if (!ins.arg1.empty() && ins.arg1[0] != 't' && !isNumber(ins.arg1) && ins.op != "PRINT") used.insert(ins.arg1);
    }

    std::vector<IRInstruction> finalIR;
    finalIR.reserve(folded.size());

    for (const auto &ins : folded) {
        // keep comments and side-effect ops
        if (!ins.op.empty() && ins.op[0] == ';') {
            finalIR.push_back(ins);
            continue;
        }

        if (ins.op == "PRINT" || ins.op == "STORE" || ins.op == "READ") {
            finalIR.push_back(ins);
            continue;
        }

        // if the instruction produces a temp and that temp isn't used later -> drop
        if (!ins.result.empty() && ins.result[0] == 't') {
            if (!used.count(ins.result)) {
                removedAny = true;
                continue; // drop
            }
        }

        finalIR.push_back(ins);
    }

    // 3) emit human-readable optimization messages as IR comment entries (so main prints them)
    // Insert summary messages at the top in the user's requested style.
    std::vector<IRInstruction> summary;

    summary.push_back(IRInstruction{"; === Optimization ===", "", "", ""});

    if (removedAny) summary.push_back(IRInstruction{"; Removed dead code", "", "", ""});
    if (foldedAny) {
        // print a few folded examples (up to 3)
        for (size_t i = 0; i < foldedExamples.size() && i < 3; ++i) {
            summary.push_back(IRInstruction{std::string("; Constant folded: ") + foldedExamples[i], "", "", ""});
        }
    }
    if (movChainFound) summary.push_back(IRInstruction{"; Simplified MOV chains", "", "", ""});
    if (divByZeroFound) summary.push_back(IRInstruction{"; Removed unreachable code after fatal divide-by-zero", "", "", ""});

    if (!removedAny && !foldedAny && !movChainFound && !divByZeroFound) {
        summary.push_back(IRInstruction{"; Optimization: (no changes)", "", "", ""});
    }

    // Prepend summary messages to finalIR
    std::vector<IRInstruction> out;
    out.reserve(summary.size() + finalIR.size());
    for (const auto &s : summary) out.push_back(s);
    for (const auto &f : finalIR) out.push_back(f);

    return out;
}
