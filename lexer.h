#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

using namespace std;

enum TokenType {
    INT, VOID, IF, ELSE, WHILE, BREAK, CONTINUE, RETURN,
    IDENT, INTCONST,
    PLUS, MINUS, MULTIPLY, DIVIDE, MODULO,
    ASSIGN, EQUAL, NOT_EQUAL,
    LESS, GREATER, LESS_EQUAL, GREATER_EQUAL,
    LOGICAL_AND, LOGICAL_OR, LOGICAL_NOT,
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACE, RIGHT_BRACE,
    SEMICOLON, COMMA,
    END_OF_FILE
};

struct Token {
    TokenType type;
    string value;
    int line;
    int column;
    
    Token(TokenType t, const string& v, int l = 0, int c = 0) 
        : type(t), value(v), line(l), column(c) {}
};

class Lexer {
private:
    string input;
    size_t pos;
    int line;
    int column;
    vector<Token> tokens;
    unordered_set<string> keywords;
    unordered_map<TokenType, string> tokenTypeNames;
    
    void initKeywords();
    void initTokenNames();
    char getCurrentChar() const;
    char getNextChar() const;
    void moveNext();
    void skipSpace();
    void skipComments();
    void skipLineComment();
    void skipBlockComment();
    Token readNum();
    Token readId();
    Token readOp();
    Token readDelim();
    bool isKeyword(const string& word) const;
    bool isLetter(char c) const;
    bool isDigit(char c) const;
    bool isSpace(char c) const;
    bool isOpStart(char c) const;
    bool isDelim(char c) const;

public:
    Lexer(const string& input);
    vector<Token> tokenize();
    void printTokens() const;
};

#endif
