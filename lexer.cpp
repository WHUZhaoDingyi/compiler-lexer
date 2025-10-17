#include "lexer.h"
#include <iostream>
#include <cctype>
#include <sstream>

using namespace std;

Lexer::Lexer(const string& input) : input(input), pos(0), line(1), column(1) 
{
    initKeywords();
    initTokenNames();
}

void Lexer::initKeywords() 
{
    keywords.insert("int");
    keywords.insert("void");
    keywords.insert("if");
    keywords.insert("else");
    keywords.insert("while");
    keywords.insert("break");
    keywords.insert("continue");
    keywords.insert("return");
}

void Lexer::initTokenNames() 
{
    tokenTypeNames[INT] = "'int'";
    tokenTypeNames[VOID] = "'void'";
    tokenTypeNames[IF] = "'if'";
    tokenTypeNames[ELSE] = "'else'";
    tokenTypeNames[WHILE] = "'while'";
    tokenTypeNames[BREAK] = "'break'";
    tokenTypeNames[CONTINUE] = "'continue'";
    tokenTypeNames[RETURN] = "'return'";
    tokenTypeNames[IDENT] = "Ident";
    tokenTypeNames[INTCONST] = "IntConst";
    tokenTypeNames[PLUS] = "'+'";
    tokenTypeNames[MINUS] = "'-'";
    tokenTypeNames[MULTIPLY] = "'*'";
    tokenTypeNames[DIVIDE] = "'/'";
    tokenTypeNames[MODULO] = "'%'";
    tokenTypeNames[ASSIGN] = "'='";
    tokenTypeNames[EQUAL] = "'=='";
    tokenTypeNames[NOT_EQUAL] = "'!='";
    tokenTypeNames[LESS] = "'<'";
    tokenTypeNames[GREATER] = "'>'";
    tokenTypeNames[LESS_EQUAL] = "'<='";
    tokenTypeNames[GREATER_EQUAL] = "'>='";
    tokenTypeNames[LOGICAL_AND] = "'&&'";
    tokenTypeNames[LOGICAL_OR] = "'||'";
    tokenTypeNames[LOGICAL_NOT] = "'!'";
    tokenTypeNames[LEFT_PAREN] = "'('";
    tokenTypeNames[RIGHT_PAREN] = "')'";
    tokenTypeNames[LEFT_BRACE] = "'{'";
    tokenTypeNames[RIGHT_BRACE] = "'}'";
    tokenTypeNames[SEMICOLON] = "';'";
    tokenTypeNames[COMMA] = "','";
    tokenTypeNames[END_OF_FILE] = "EOF";
}

char Lexer::getCurrentChar() const 
{
    if (pos >= input.length()) 
        return '\0';
    return input[pos];
}

char Lexer::getNextChar() const 
{
    if (pos + 1 >= input.length()) 
        return '\0';
    return input[pos + 1];
}

void Lexer::moveNext() 
{
    if (pos < input.length()) 
    {
        if (input[pos] == '\n') 
        {
            line++;
            column = 1;
        } 
        else 
        {
            column++;
        }
        pos++;
    }
}

void Lexer::skipSpace() 
{
    while (pos < input.length() && isSpace(getCurrentChar())) 
    {
        moveNext();
    }
}

void Lexer::skipComments() 
{
    if (getCurrentChar() == '/' && getNextChar() == '/') 
    {
        skipLineComment();
    } 
    else if (getCurrentChar() == '/' && getNextChar() == '*') 
    {
        skipBlockComment();
    }
}

void Lexer::skipLineComment() 
{
    moveNext();
    moveNext();
    while (pos < input.length() && getCurrentChar() != '\n') 
    {
        moveNext();
    }
}

void Lexer::skipBlockComment() 
{
    moveNext();
    moveNext();
    while (pos < input.length()) 
    {
        if (getCurrentChar() == '*' && getNextChar() == '/') 
        {
            moveNext();
            moveNext();
            break;
        }
        moveNext();
    }
}

Token Lexer::readNum() 
{
    string value;
    int startLine = line;
    int startColumn = column;
    
    if (getCurrentChar() == '-') 
    {
        value += getCurrentChar();
        moveNext();
    }
    
    if (getCurrentChar() == '0') 
    {
        value += getCurrentChar();
        moveNext();
    } 
    else if (isDigit(getCurrentChar())) 
    {
        while (isDigit(getCurrentChar())) 
        {
            value += getCurrentChar();
            moveNext();
        }
    }
    
    return Token(INTCONST, value, startLine, startColumn);
}

Token Lexer::readId() 
{
    string value;
    int startLine = line;
    int startColumn = column;
    
    while (pos < input.length() && (isLetter(getCurrentChar()) || isDigit(getCurrentChar()))) 
    {
        value += getCurrentChar();
        moveNext();
    }
    
    if (isKeyword(value)) 
    {
        TokenType tokenType = IDENT;
        if (value == "int") tokenType = INT;
        else if (value == "void") tokenType = VOID;
        else if (value == "if") tokenType = IF;
        else if (value == "else") tokenType = ELSE;
        else if (value == "while") tokenType = WHILE;
        else if (value == "break") tokenType = BREAK;
        else if (value == "continue") tokenType = CONTINUE;
        else if (value == "return") tokenType = RETURN;
        
        return Token(tokenType, value, startLine, startColumn);
    }
    
    return Token(IDENT, value, startLine, startColumn);
}

Token Lexer::readOp() 
{
    int startLine = line;
    int startColumn = column;
    
    char first = getCurrentChar();
    moveNext();
    
    if (pos < input.length()) 
    {
        char next = getCurrentChar();
        string op = string(1, first) + next;
        
        if (op == "==") 
        {
            moveNext();
            return Token(EQUAL, op, startLine, startColumn);
        } 
        else if (op == "!=") 
        {
            moveNext();
            return Token(NOT_EQUAL, op, startLine, startColumn);
        } 
        else if (op == "<=") 
        {
            moveNext();
            return Token(LESS_EQUAL, op, startLine, startColumn);
        } 
        else if (op == ">=") 
        {
            moveNext();
            return Token(GREATER_EQUAL, op, startLine, startColumn);
        } 
        else if (op == "&&") 
        {
            moveNext();
            return Token(LOGICAL_AND, op, startLine, startColumn);
        } 
        else if (op == "||") 
        {
            moveNext();
            return Token(LOGICAL_OR, op, startLine, startColumn);
        }
    }
    
    switch (first) 
    {
        case '+': return Token(PLUS, string(1, first), startLine, startColumn);
        case '-': return Token(MINUS, string(1, first), startLine, startColumn);
        case '*': return Token(MULTIPLY, string(1, first), startLine, startColumn);
        case '/': return Token(DIVIDE, string(1, first), startLine, startColumn);
        case '%': return Token(MODULO, string(1, first), startLine, startColumn);
        case '=': return Token(ASSIGN, string(1, first), startLine, startColumn);
        case '<': return Token(LESS, string(1, first), startLine, startColumn);
        case '>': return Token(GREATER, string(1, first), startLine, startColumn);
        case '!': return Token(LOGICAL_NOT, string(1, first), startLine, startColumn);
        default: return Token(END_OF_FILE, "", startLine, startColumn);
    }
}

Token Lexer::readDelim() 
{
    int startLine = line;
    int startColumn = column;
    
    char c = getCurrentChar();
    moveNext();
    
    switch (c) 
    {
        case '(': return Token(LEFT_PAREN, string(1, c), startLine, startColumn);
        case ')': return Token(RIGHT_PAREN, string(1, c), startLine, startColumn);
        case '{': return Token(LEFT_BRACE, string(1, c), startLine, startColumn);
        case '}': return Token(RIGHT_BRACE, string(1, c), startLine, startColumn);
        case ';': return Token(SEMICOLON, string(1, c), startLine, startColumn);
        case ',': return Token(COMMA, string(1, c), startLine, startColumn);
        default: return Token(END_OF_FILE, "", startLine, startColumn);
    }
}

bool Lexer::isKeyword(const string& word) const 
{
    return keywords.find(word) != keywords.end();
}

bool Lexer::isLetter(char c) const 
{
    return isalpha(c) || c == '_';
}

bool Lexer::isDigit(char c) const 
{
    return isdigit(c);
}

bool Lexer::isSpace(char c) const 
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool Lexer::isOpStart(char c) const 
{
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
           c == '=' || c == '!' || c == '<' || c == '>' || c == '&' || c == '|';
}

bool Lexer::isDelim(char c) const 
{
    return c == '(' || c == ')' || c == '{' || c == '}' || 
           c == ';' || c == ',';
}

vector<Token> Lexer::tokenize() 
{
    tokens.clear();
    
    while (pos < input.length()) 
    {
        skipSpace();
        
        if (pos >= input.length()) 
            break;
        
        if (getCurrentChar() == '/' && (getNextChar() == '/' || getNextChar() == '*')) 
        {
            skipComments();
            continue;
        }
        
        char ch = getCurrentChar();
        
        if (isLetter(ch)) 
        {
            tokens.push_back(readId());
        } 
        else if (isDigit(ch) || (ch == '-' && isDigit(getNextChar()))) 
        {
            tokens.push_back(readNum());
        } 
        else if (isOpStart(ch)) 
        {
            tokens.push_back(readOp());
        } 
        else if (isDelim(ch)) 
        {
            tokens.push_back(readDelim());
        } 
        else 
        {
            moveNext();
        }
    }
    
    return tokens;
}

void Lexer::printTokens() const 
{
    for (size_t i = 0; i < tokens.size(); ++i) 
    {
        const Token& t = tokens[i];
        cout << i << ":" << tokenTypeNames.at(t.type) << ":\"" << t.value << "\"" << endl;
    }
}
