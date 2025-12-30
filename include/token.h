#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum TokenType {
    NUMBER,
    STRING,
    IDENTIFIER,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    ASSIGN,
    LPAREN,
    RPAREN,
    SEMICOLON,
    COUT,
    CIN,
    END,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int line;

    Token(TokenType type, std::string value = "", int line = 0)
        : type(type), value(value), line(line) {}
};

#endif
