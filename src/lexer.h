#pragma once

#include "tokens.h"
#include <string>
#include <vector>

class Lexer {
public:
    explicit Lexer(std::string input);
    std::vector<Token> lexAll(std::vector<int>& errorLines, std::vector<std::string>& errorMsgs);

private:
    char peek() const;
    char peekNext() const;
    char get();
    bool match(char c);
    void skipWhitespaceAndComments(std::vector<int>& errorLines, std::vector<std::string>& errorMsgs);
    void lexIdentifierOrKeyword(std::vector<Token>& out);
    void lexNumber(std::vector<Token>& out);
    void add(TokenKind k, std::vector<Token>& out);

private:
    std::string src;
    size_t pos {0};
    int line {1};
};


