#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include <string>

class Lexer {
private:
    std::string text;
    size_t pos; // tracks current position in the input text
    int line;
    char currentChar();

    void skipWhitespaceAndComments();
    std::string number();
    std::string identifier();
    std::string stringLiteral();

public:
    Lexer(const std::string& text);
    Token getNextToken();
};

#endif
