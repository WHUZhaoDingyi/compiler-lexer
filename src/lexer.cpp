#include "lexer.h"
#include <cctype>

Lexer::Lexer(std::string input) : src(std::move(input)) {}

char Lexer::peek() const {
    if (pos >= src.size()) return '\0';
    return src[pos];
}

char Lexer::peekNext() const {
    if (pos + 1 >= src.size()) return '\0';
    return src[pos + 1];
}

char Lexer::get() {
    if (pos >= src.size()) return '\0';
    char c = src[pos++];
    if (c == '\n') line++;
    return c;
}

bool Lexer::match(char c) {
    if (peek() == c) { get(); return true; }
    return false;
}

void Lexer::add(TokenKind k, std::vector<Token>& out) {
    Token t; t.kind = k; t.line = line; out.push_back(std::move(t));
}

void Lexer::lexIdentifierOrKeyword(std::vector<Token>& out) {
    std::string s;
    int startLine = line;
    s.push_back(get());
    while (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_') {
        s.push_back(get());
    }
    Token t; t.lexeme = s; t.line = startLine;
    if (s == "int") t.kind = TokenKind::KwInt;
    else if (s == "void") t.kind = TokenKind::KwVoid;
    else if (s == "if") t.kind = TokenKind::KwIf;
    else if (s == "else") t.kind = TokenKind::KwElse;
    else if (s == "while") t.kind = TokenKind::KwWhile;
    else if (s == "break") t.kind = TokenKind::KwBreak;
    else if (s == "continue") t.kind = TokenKind::KwContinue;
    else if (s == "return") t.kind = TokenKind::KwReturn;
    else t.kind = TokenKind::Identifier;
    out.push_back(std::move(t));
}

void Lexer::lexNumber(std::vector<Token>& out) {
    std::string s;
    int startLine = line;
    if (peek() == '-') {
        s.push_back(get());
    }
    if (peek() == '0') {
        s.push_back(get());
    } else if (std::isdigit(static_cast<unsigned char>(peek()))) {
        while (std::isdigit(static_cast<unsigned char>(peek()))) s.push_back(get());
    } else {
        Token t; t.kind = TokenKind::Minus; t.line = startLine; out.push_back(std::move(t));
        return;
    }
    while (std::isdigit(static_cast<unsigned char>(peek()))) s.push_back(get());
    Token t; t.kind = TokenKind::Number; t.lexeme = s; t.line = startLine; out.push_back(std::move(t));
}

void Lexer::skipWhitespaceAndComments(std::vector<int>& errorLines, std::vector<std::string>& errorMsgs) {
    for (;;) {
        while (std::isspace(static_cast<unsigned char>(peek()))) get();
        if (peek() == '/' && peekNext() == '/') {
            while (peek() && peek() != '\n') get();
            continue;
        }
        if (peek() == '/' && peekNext() == '*') {
            int start = line;
            get(); get();
            bool closed = false;
            while (peek()) {
                if (peek() == '*' && peekNext() == '/') { get(); get(); closed = true; break; }
                get();
            }
            if (!closed) {
                errorLines.push_back(start);
                errorMsgs.emplace_back("Unterminated comment");
            }
            continue;
        }
        break;
    }
}

std::vector<Token> Lexer::lexAll(std::vector<int>& errorLines, std::vector<std::string>& errorMsgs) {
    std::vector<Token> out;
    while (true) {
        skipWhitespaceAndComments(errorLines, errorMsgs);
        char c = peek();
        if (c == '\0') { Token t; t.kind = TokenKind::EndOfFile; t.line = line; out.push_back(std::move(t)); break; }

        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            lexIdentifierOrKeyword(out);
            continue;
        }
        if (std::isdigit(static_cast<unsigned char>(c)) || (c=='-' && std::isdigit(static_cast<unsigned char>(peekNext())))) {
            lexNumber(out);
            continue;
        }

        int tokLine = line;
        switch (c) {
            case '(': get(); add(TokenKind::LParen, out); break;
            case ')': get(); add(TokenKind::RParen, out); break;
            case '{': get(); add(TokenKind::LBrace, out); break;
            case '}': get(); add(TokenKind::RBrace, out); break;
            case ',': get(); add(TokenKind::Comma, out); break;
            case ';': get(); add(TokenKind::Semicolon, out); break;
            case '+': get(); add(TokenKind::Plus, out); break;
            case '-': get(); add(TokenKind::Minus, out); break;
            case '*': get(); add(TokenKind::Star, out); break;
            case '%': get(); add(TokenKind::Percent, out); break;
            case '!': {
                get();
                if (match('=')) { Token t; t.kind = TokenKind::NotEq; t.line = tokLine; out.push_back(std::move(t)); }
                else { Token t; t.kind = TokenKind::Not; t.line = tokLine; out.push_back(std::move(t)); }
                break;
            }
            case '=': {
                get();
                if (match('=')) { Token t; t.kind = TokenKind::EqEq; t.line = tokLine; out.push_back(std::move(t)); }
                else { Token t; t.kind = TokenKind::Assign; t.line = tokLine; out.push_back(std::move(t)); }
                break;
            }
            case '&': {
                get();
                if (match('&')) { Token t; t.kind = TokenKind::AndAnd; t.line = tokLine; out.push_back(std::move(t)); }
                else { Token t; t.kind = TokenKind::Invalid; t.line = tokLine; out.push_back(std::move(t)); errorLines.push_back(tokLine); errorMsgs.emplace_back("Invalid '&'"); }
                break;
            }
            case '|': {
                get();
                if (match('|')) { Token t; t.kind = TokenKind::OrOr; t.line = tokLine; out.push_back(std::move(t)); }
                else { Token t; t.kind = TokenKind::Invalid; t.line = tokLine; out.push_back(std::move(t)); errorLines.push_back(tokLine); errorMsgs.emplace_back("Invalid '|' "); }
                break;
            }
            case '<': {
                get();
                if (match('=')) { Token t; t.kind = TokenKind::LessEq; t.line = tokLine; out.push_back(std::move(t)); }
                else { Token t; t.kind = TokenKind::Less; t.line = tokLine; out.push_back(std::move(t)); }
                break;
            }
            case '>': {
                get();
                if (match('=')) { Token t; t.kind = TokenKind::GreaterEq; t.line = tokLine; out.push_back(std::move(t)); }
                else { Token t; t.kind = TokenKind::Greater; t.line = tokLine; out.push_back(std::move(t)); }
                break;
            }
            case '/': {
                get(); add(TokenKind::Slash, out); break;
            }
            default: {
                get();
                Token t; t.kind = TokenKind::Invalid; t.line = tokLine; out.push_back(std::move(t));
                errorLines.push_back(tokLine);
                errorMsgs.emplace_back("Invalid character");
                break;
            }
        }
    }
    return out;
}


