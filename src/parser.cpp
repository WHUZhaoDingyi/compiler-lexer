#include "parser.h"

Parser::Parser(const std::vector<Token> &ts)
    : tokens(ts), idx(0), loopDepth(0), hasError(false) {}

const std::map<int, std::string> &Parser::getErrors() const {
    return parseErrors;
}

const Token &Parser::current(int offset) const {
    std::size_t pos = idx + static_cast<std::size_t>(offset);
    if (pos >= tokens.size()) return tokens.back();
    return tokens[pos];
}

bool Parser::is(Tag k, int offset) const {
    return current(offset).kind == k;
}

void Parser::advance() {
    if (idx < tokens.size()) ++idx;
}

void Parser::recordError(const std::string &msg) {
    hasError = true;
    int ln = current().line;
    if (parseErrors.find(ln) == parseErrors.end()) {
        parseErrors[ln] = msg;
    }
}

bool Parser::accept(Tag k) {
    if (is(k)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::expect(Tag k, const std::string &msg) {
    if (is(k)) {
        advance();
        return true;
    }
    recordError(msg);
    return false;
}

void Parser::sync() {
    while (!is(Tag::End) && !is(Tag::Semicolon) && !is(Tag::RBrace)) {
        advance();
    }
    if (is(Tag::Semicolon)) advance();
}

bool Parser::parse() {
    parseCompUnit();
    return !hasError;
}

/* CompUnit → FuncDef+ */
void Parser::parseCompUnit() {
    while (!is(Tag::End)) {
        parseFuncDef();
    }
}

/* FuncDef → ("int" | "void") ID "(" (Param ("," Param)*)? ")" Block */
void Parser::parseFuncDef() {
    if (!is(Tag::KwInt) && !is(Tag::KwVoid)) {
        recordError("Expected function return type");
        sync();
        if (is(Tag::RBrace)) advance();
        return;
    }
    advance(); // return type

    if (!expect(Tag::Id, "Expected function name")) {
        sync();
        if (is(Tag::RBrace)) advance();
        return;
    }

    expect(Tag::LParen, "Lack of '('");

    if (is(Tag::KwInt)) {
        parseParam();
        while (accept(Tag::Comma)) {
            parseParam();
        }
    }

    expect(Tag::RParen, "Lack of ')'");
    parseBlock();
}

/* Param → "int" ID */
void Parser::parseParam() {
    expect(Tag::KwInt, "Expected int");
    expect(Tag::Id, "Expected identifier");
}

/* Block → "{" Stmt* "}" */
void Parser::parseBlock() {
    if (!expect(Tag::LBrace, "Lack of '{'")) return;

    while (!is(Tag::RBrace) && !is(Tag::End)) {
        parseStmt();
    }

    expect(Tag::RBrace, "Lack of '}'");
}

/* Stmt 各种语句 */
void Parser::parseStmt() {
    // 声明语句
    if (is(Tag::KwInt)) {
        advance(); // int
        expect(Tag::Id, "Expected identifier");
        if (accept(Tag::Assign)) {
            parseExpr();
        }
        // 支持 int a=1, b=2;（虽然文法不写，但不影响测例）
        while (accept(Tag::Comma)) {
            expect(Tag::Id, "Expected identifier");
            if (accept(Tag::Assign)) {
                parseExpr();
            }
        }
        expect(Tag::Semicolon, "Lack of ';'");
        return;
    }

    // if
    if (is(Tag::KwIf)) {
        advance();
        expect(Tag::LParen, "Lack of '('");
        parseExpr();
        expect(Tag::RParen, "Lack of ')'");
        parseStmt();
        if (accept(Tag::KwElse)) {
            parseStmt();
        }
        return;
    }

    // while
    if (is(Tag::KwWhile)) {
        advance();
        expect(Tag::LParen, "Lack of '('");
        parseExpr();
        expect(Tag::RParen, "Lack of ')'");
        ++loopDepth;
        parseStmt();
        --loopDepth;
        return;
    }

    // break
    if (is(Tag::KwBreak)) {
        advance();
        expect(Tag::Semicolon, "Lack of ';'");
        return;
    }

    // continue
    if (is(Tag::KwContinue)) {
        advance();
        expect(Tag::Semicolon, "Lack of ';'");
        return;
    }

    // return
    if (is(Tag::KwReturn)) {
        advance();
        if (!is(Tag::Semicolon)) {
            parseExpr();
        }
        expect(Tag::Semicolon, "Lack of ';'");
        return;
    }

    // 语句块
    if (is(Tag::LBrace)) {
        parseBlock();
        return;
    }

    // 以标识符开头：赋值 or 函数调用
    if (is(Tag::Id)) {
        advance();
        if (accept(Tag::Assign)) {
            parseExpr();
            expect(Tag::Semicolon, "Lack of ';'");
        } else if (accept(Tag::LParen)) {
            if (!is(Tag::RParen)) {
                parseExpr();
                while (accept(Tag::Comma)) {
                    parseExpr();
                }
            }
            expect(Tag::RParen, "Lack of ')'");
            expect(Tag::Semicolon, "Lack of ';'");
        } else {
            expect(Tag::Semicolon, "Lack of ';'");
        }
        return;
    }

    // 空语句
    if (is(Tag::Semicolon)) {
        advance();
        return;
    }

    // 其他：意外 token
    recordError("Unexpected token");
    advance();
}

/* Expr → LOrExpr */
void Parser::parseExpr() {
    parseLOrExpr();
}

/* LOrExpr → LAndExpr | LOrExpr "||" LAndExpr */
void Parser::parseLOrExpr() {
    parseLAndExpr();
    while (is(Tag::OrOr)) {
        advance();
        parseLAndExpr();
    }
}

/* LAndExpr → RelExpr | LAndExpr "&&" RelExpr */
void Parser::parseLAndExpr() {
    parseRelExpr();
    while (is(Tag::AndAnd)) {
        advance();
        parseRelExpr();
    }
}

/* RelExpr → AddExpr | RelExpr ( < > <= >= == != ) AddExpr */
void Parser::parseRelExpr() {
    parseAddExpr();
    while (is(Tag::Lt) || is(Tag::Le) || is(Tag::Gt) || is(Tag::Ge)
           || is(Tag::Eq) || is(Tag::Ne)) {
        advance();
        parseAddExpr();
    }
}

/* AddExpr → MulExpr | AddExpr ("+" | "-") MulExpr */
void Parser::parseAddExpr() {
    parseMulExpr();
    while (is(Tag::Plus) || is(Tag::Minus)) {
        advance();
        parseMulExpr();
    }
}

/* MulExpr → UnaryExpr | MulExpr ("*" | "/" | "%") UnaryExpr */
void Parser::parseMulExpr() {
    parseUnaryExpr();
    while (is(Tag::Star) || is(Tag::Slash) || is(Tag::Percent)) {
        advance();
        parseUnaryExpr();
    }
}

/* UnaryExpr → PrimaryExpr | ("+" | "-" | "!") UnaryExpr */
void Parser::parseUnaryExpr() {
    if (is(Tag::Plus) || is(Tag::Minus) || is(Tag::Not)) {
        advance();
        parseUnaryExpr();
    } else {
        parsePrimaryExpr();
    }
}

/* PrimaryExpr → ID | NUMBER | "(" Expr ")" | ID "(" (Expr ("," Expr)*)? ")" */
void Parser::parsePrimaryExpr() {
    if (is(Tag::Id)) {
        advance();
        if (accept(Tag::LParen)) {
            if (!is(Tag::RParen)) {
                parseExpr();
                while (accept(Tag::Comma)) {
                    parseExpr();
                }
            }
            expect(Tag::RParen, "Lack of ')'");
        }
    } else if (is(Tag::Number)) {
        advance();
    } else if (accept(Tag::LParen)) {
        parseExpr();
        expect(Tag::RParen, "Lack of ')'");
    } else {
        recordError("Expected expression");
        if (!is(Tag::End) && !is(Tag::Semicolon)) {
            advance();
        }
    }
}
