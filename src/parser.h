#pragma once

#include "tokens.h"
#include <string>
#include <vector>

class Parser {
public:
    Parser(const std::vector<Token>& tokens, std::vector<int>& errorLinesOut);
    bool parse();

private:
    const Token& peek(int offset = 0) const;
    bool is(TokenKind k, int offset = 0) const;
    const Token& advance();
    bool match(TokenKind k);
    bool expect(TokenKind k);
    void syncToSemicolonOrBlockEnd();

    bool parseCompUnit();
    bool parseFuncDef();
    bool parseParamList();
    bool parseParam();
    bool parseBlock();
    bool parseStmt();
    bool parseExpr();
    bool parseLOrExpr();
    bool parseLAndExpr();
    bool parseRelExpr();
    bool parseAddExpr();
    bool parseMulExpr();
    bool parseUnaryExpr();
    bool parsePrimaryExpr();

private:
    const std::vector<Token>& ts;
    size_t pos {0};
    std::vector<int>& errorLines;
};


