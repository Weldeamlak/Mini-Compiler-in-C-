#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <vector>
#include <string>

// AST node fields:
// - type: node type ("number","string","variable","binop","assign","cin","cout")
// - name: identifier name (for variable, assign target, cin)
// - value: literal value for number/string
// - op: operator for binop
// - left/right: child nodes
// - line: source line number for errors
struct ASTNode {
    std::string type;
    std::string name;
    std::string value;
    std::string op;
    ASTNode* left;
    ASTNode* right;
    int line;

    ASTNode(std::string type,
            std::string name = "",
            std::string value = "",
            std::string op = "",
            ASTNode* left = nullptr,
            ASTNode* right = nullptr,
            int line = 0)
        : type(type), name(name), value(value), op(op), left(left), right(right), line(line) {}
};

class Parser {
private:
    Lexer lexer;
    Token currentToken;

    void eat(TokenType type);
    ASTNode* factor();
    ASTNode* term();
    ASTNode* expr();
    ASTNode* assignment();
    ASTNode* statement();

public:
    Parser(Lexer lexer);
    std::vector<ASTNode*> parse();
};

#endif
