#include "parser.h"
#include <iostream>
#include <stdexcept>

// Constructor
Parser::Parser(Lexer lexer) : lexer(lexer), currentToken(this->lexer.getNextToken()) {}

// Consume current token if it matches type
void Parser::eat(TokenType type) {
    if (currentToken.type == type)
        currentToken = lexer.getNextToken();
    else
        throw std::runtime_error("Unexpected token: " + currentToken.value +
                                 " at line " + std::to_string(currentToken.line));
}

// Parse numbers, strings, variables, parentheses, unary minus
ASTNode* Parser::factor() {
    Token token = currentToken;

    if (token.type == NUMBER) {
        eat(NUMBER);
        return new ASTNode("number", "", token.value, "", nullptr, nullptr, token.line);
    }
    if (token.type == STRING) {
        eat(STRING);
        return new ASTNode("string", "", token.value, "", nullptr, nullptr, token.line);
    }
    if (token.type == IDENTIFIER) {
        eat(IDENTIFIER);
        return new ASTNode("variable", token.value, "", "", nullptr, nullptr, token.line);
    }
    if (token.type == LPAREN) {
        eat(LPAREN);
        ASTNode* node = expr();
        eat(RPAREN);
        return node;
    }
    if (token.type == MINUS) {  // unary minus
        eat(MINUS);
        ASTNode* node = factor();
        // create a binary subtraction from 0 - node
        ASTNode* zero = new ASTNode("number", "", "0", "", nullptr, nullptr, token.line);
        return new ASTNode("binop", "", "", "-", zero, node, token.line);
    }

    throw std::runtime_error("Invalid factor: " + token.value + " at line " + std::to_string(token.line));
}

// Parse *, /
ASTNode* Parser::term() {
    ASTNode* node = factor();

    while (currentToken.type == STAR || currentToken.type == SLASH) {
        Token op = currentToken;
        eat(op.type);
        ASTNode* rightNode = factor();
        node = new ASTNode("binop", "", "", op.value, node, rightNode, op.line);
    }

    return node;
}

// Parse +, -
ASTNode* Parser::expr() {
    ASTNode* node = term();

    while (currentToken.type == PLUS || currentToken.type == MINUS) {
        Token op = currentToken;
        eat(op.type);
        ASTNode* rightNode = term();
        node = new ASTNode("binop", "", "", op.value, node, rightNode, op.line);
    }

    return node;
}

// Parse variable assignment
ASTNode* Parser::assignment() {
    if (currentToken.type == IDENTIFIER) {
        std::string varName = currentToken.value;
        int lineNum = currentToken.line;  // capture line of variable
        eat(IDENTIFIER);

        if (currentToken.type == ASSIGN) {
            eat(ASSIGN);
            ASTNode* valueNode = expr();
            eat(SEMICOLON);
            return new ASTNode("assign", varName, "", "", valueNode, nullptr, lineNum);
        } else {
            // not an assignment; rollback not implemented, so treat as error
            throw std::runtime_error("Expected '=' after identifier at line " + std::to_string(lineNum));
        }
    }

    return nullptr;
}

// Parse statements: assignment, cin, cout
ASTNode* Parser::statement() {
    if (currentToken.type == CIN) {
        int lineNum = currentToken.line;
        eat(CIN);
        eat(LPAREN);

        if (currentToken.type != IDENTIFIER)
            throw std::runtime_error("Expected variable name in cin at line " + std::to_string(currentToken.line));

        std::string varName = currentToken.value;
        eat(IDENTIFIER);
        eat(RPAREN);
        eat(SEMICOLON);
        return new ASTNode("cin", varName, "", "", nullptr, nullptr, lineNum);
    }

    if (currentToken.type == COUT) {
        int lineNum = currentToken.line;
        eat(COUT);
        eat(LPAREN);
        ASTNode* exprNode = expr();
        eat(RPAREN);
        eat(SEMICOLON);
        return new ASTNode("cout", "", "", "", exprNode, nullptr, lineNum);
    }

    ASTNode* assignNode = nullptr;
    // Try to parse assignment; if it throws, let parse() catch and recover
    assignNode = assignment();
    if (assignNode != nullptr) return assignNode;

    throw std::runtime_error("Unknown statement at line " + std::to_string(currentToken.line));
}

// Parse all statements in input
std::vector<ASTNode*> Parser::parse() {
    std::vector<ASTNode*> nodes;
    while (currentToken.type != END) {
        try {
            nodes.push_back(statement());
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            // Skip to next semicolon to continue parsing
            while (currentToken.type != SEMICOLON && currentToken.type != END)
                currentToken = lexer.getNextToken();
            if (currentToken.type == SEMICOLON) currentToken = lexer.getNextToken();
        }
    }
    return nodes;
}
