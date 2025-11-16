#ifndef TOYC_PARSER_H
#define TOYC_PARSER_H

#include <vector>
#include <map>
#include <string>
#include "tokens.h"

class Parser {
public:
    explicit Parser(const std::vector<Token> &ts);

    bool parse();
    const std::map<int, std::string> &getErrors() const;

private:
    std::vector<Token> tokens;
    std::size_t idx;
    int loopDepth;
    bool hasError;
    std::map<int, std::string> parseErrors;

    const Token &current(int offset = 0) const;
    bool is(Tag k, int offset = 0) const;
    void advance();
    void recordError(const std::string &msg);
    bool accept(Tag k);
    bool expect(Tag k, const std::string &msg);
    void sync();

    void parseCompUnit();
    void parseFuncDef();
    void parseParam();
    void parseBlock();
    void parseStmt();
    void parseExpr();
    void parseLOrExpr();
    void parseLAndExpr();
    void parseRelExpr();
    void parseAddExpr();
    void parseMulExpr();
    void parseUnaryExpr();
    void parsePrimaryExpr();
};

#endif // TOYC_PARSER_H
