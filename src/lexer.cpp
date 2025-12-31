#include "lexer.h"
#include <cctype>
#include <stdexcept>

Lexer::Lexer(const std::string& text) : text(text), pos(0), line(1) {}

char Lexer::currentChar() {
    return pos < text.size() ? text[pos] : '\0';
}

void Lexer::skipWhitespaceAndComments() {
    while (pos < text.size()) {
        char c = text[pos];
        // whitespace
        if (std::isspace(static_cast<unsigned char>(c))) {
            if (c == '\n') line++;
            pos++;
            continue;
        }

        // Single-line comment //
        if (c == '/' && pos + 1 < text.size() && text[pos + 1] == '/') {
            pos += 2;
            while (pos < text.size() && text[pos] != '\n') pos++;
            continue;
        }

        // Multi-line comment /* ... */
        if (c == '/' && pos + 1 < text.size() && text[pos + 1] == '*') {
            pos += 2;
            bool closed = false;
            while (pos + 1 < text.size()) {
                if (text[pos] == '\n') line++;
                if (text[pos] == '*' && text[pos + 1] == '/') {
                    pos += 2;
                    closed = true;
                    break;
                }
                pos++;
            }
            if (!closed) {
                throw std::runtime_error("Unterminated comment starting at line " + std::to_string(line));
            }
            continue;
        }

        break;
    }
}

std::string Lexer::number() {
    std::string result;
    while (std::isdigit(static_cast<unsigned char>(currentChar()))) {
        result += currentChar();
        pos++;
    }
    return result;
}
//identifier
std::string Lexer::identifier() {
    std::string result;
    // allow letters, digits and underscore; identifiers may start with underscore
    while (std::isalnum(static_cast<unsigned char>(currentChar())) || currentChar() == '_') {
        result += currentChar();
        pos++;
    }
    return result;
}

std::string Lexer::stringLiteral() {
    // supports simple string literal without escape processing
    pos++; // skip opening "
    std::string result;
    while (currentChar() != '"' && currentChar() != '\0') {
        if (currentChar() == '\n') line++;
        result += currentChar();
        pos++;
    }
    if (currentChar() == '"') {
        pos++; // skip closing "
        return result;
    }
    throw std::runtime_error("Unterminated string literal at line " + std::to_string(line));
}

Token Lexer::getNextToken() {
    skipWhitespaceAndComments();

    char c = currentChar();
    if (c == '\0') return Token(END, "", line);

    if (std::isdigit(static_cast<unsigned char>(c))) {
        return Token(NUMBER, number(), line);
    }

    if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
        std::string id = identifier();
        if (id == "cout") return Token(COUT, id, line);
        if (id == "cin") return Token(CIN, id, line);
        return Token(IDENTIFIER, id, line);
    }

    if (c == '"') return Token(STRING, stringLiteral(), line);

    // Single-char tokens
    pos++;
    switch (c) {
        case '+': return Token(PLUS, "+", line);
        case '-': return Token(MINUS, "-", line);
        case '*': return Token(STAR, "*", line);
        case '/': return Token(SLASH, "/", line);
        case '=': return Token(ASSIGN, "=", line);
        case '(': return Token(LPAREN, "(", line);
        case ')': return Token(RPAREN, ")", line);
        case ';': return Token(SEMICOLON, ";", line);
        default:
            throw std::runtime_error(std::string("Invalid character '") + c + "' at line " + std::to_string(line));
    }
}
