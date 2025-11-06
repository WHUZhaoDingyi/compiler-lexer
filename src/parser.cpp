#include "parser.h"

Parser::Parser(const std::vector<Token>& tokens, std::vector<int>& errorLinesOut)
    : ts(tokens), errorLines(errorLinesOut) {}

const Token& Parser::peek(int offset) const {
    size_t i = pos + static_cast<size_t>(offset);
    if (i >= ts.size()) return ts.back();
    return ts[i];
}

bool Parser::is(TokenKind k, int offset) const {
    return peek(offset).kind == k;
}

const Token& Parser::advance() {
    const Token& t = peek();
    if (pos < ts.size()) pos++;
    return t;
}

bool Parser::match(TokenKind k) {
    if (is(k)) { advance(); return true; }
    return false;
}

bool Parser::expect(TokenKind k) {
    if (match(k)) return true;
    errorLines.push_back(peek().line);
    // best-effort recovery
    if (k == TokenKind::Semicolon) {
        syncToSemicolonOrBlockEnd();
        if (is(TokenKind::Semicolon)) advance();
        return false;
    }
    return false;
}

void Parser::syncToSemicolonOrBlockEnd() {
    while (!is(TokenKind::EndOfFile) && !is(TokenKind::Semicolon) && !is(TokenKind::RBrace)) {
        advance();
    }
}

bool Parser::parse() {
    bool ok = parseCompUnit();
    return ok && errorLines.empty();
}

bool Parser::parseCompUnit() {
    bool any = false;
    while (!is(TokenKind::EndOfFile)) {
        if (!parseFuncDef()) {
            errorLines.push_back(peek().line);
            syncToSemicolonOrBlockEnd();
            if (is(TokenKind::Semicolon)) advance();
            else if (is(TokenKind::RBrace)) advance();
            else if (is(TokenKind::EndOfFile)) break;
        } else {
            any = true;
        }
    }
    return any;
}

bool Parser::parseFuncDef() {
    if (!(is(TokenKind::KwInt) || is(TokenKind::KwVoid))) return false;
    advance();
    if (!match(TokenKind::Identifier)) { errorLines.push_back(peek().line); return false; }
    if (!expect(TokenKind::LParen)) return false;
    if (!is(TokenKind::RParen)) {
        if (!parseParam()) return false;
        while (match(TokenKind::Comma)) {
            if (!parseParam()) return false;
        }
    }
    if (!expect(TokenKind::RParen)) return false;
    if (!parseBlock()) return false;
    return true;
}

bool Parser::parseParam() {
    if (!match(TokenKind::KwInt)) { errorLines.push_back(peek().line); return false; }
    if (!match(TokenKind::Identifier)) { errorLines.push_back(peek().line); return false; }
    return true;
}

bool Parser::parseBlock() {
    if (!match(TokenKind::LBrace)) { errorLines.push_back(peek().line); return false; }
    while (!is(TokenKind::RBrace) && !is(TokenKind::EndOfFile)) {
        if (!parseStmt()) {
            errorLines.push_back(peek().line);
            syncToSemicolonOrBlockEnd();
            if (is(TokenKind::Semicolon)) advance();
        }
    }
    if (!expect(TokenKind::RBrace)) return false;
    return true;
}

bool Parser::parseStmt() {
    if (is(TokenKind::LBrace)) return parseBlock();
    if (match(TokenKind::Semicolon)) return true;

    if (is(TokenKind::KwInt)) {
        advance();
        if (!match(TokenKind::Identifier)) { errorLines.push_back(peek().line); return false; }
        if (!expect(TokenKind::Assign)) return false;
        if (!parseExpr()) return false;
        if (!expect(TokenKind::Semicolon)) return false;
        return true;
    }

    if (is(TokenKind::KwIf)) {
        advance();
        if (!expect(TokenKind::LParen)) return false;
        if (!parseExpr()) return false;
        if (!expect(TokenKind::RParen)) return false;
        if (!parseStmt()) return false;
        if (match(TokenKind::KwElse)) {
            if (!parseStmt()) return false;
        }
        return true;
    }

    if (is(TokenKind::KwWhile)) {
        advance();
        if (!expect(TokenKind::LParen)) return false;
        if (!parseExpr()) return false;
        if (!expect(TokenKind::RParen)) return false;
        if (!parseStmt()) return false;
        return true;
    }

    if (is(TokenKind::KwBreak)) { advance(); if (!expect(TokenKind::Semicolon)) return false; return true; }
    if (is(TokenKind::KwContinue)) { advance(); if (!expect(TokenKind::Semicolon)) return false; return true; }
    if (is(TokenKind::KwReturn)) { advance(); if (!parseExpr()) return false; if (!expect(TokenKind::Semicolon)) return false; return true; }

    if (is(TokenKind::Identifier) && peek(1).kind == TokenKind::Assign) {
        advance();
        advance();
        if (!parseExpr()) return false;
        if (!expect(TokenKind::Semicolon)) return false;
        return true;
    }

    if (!parseExpr()) return false;
    if (!expect(TokenKind::Semicolon)) return false;
    return true;
}

bool Parser::parseExpr() { return parseLOrExpr(); }

bool Parser::parseLOrExpr() {
    if (!parseLAndExpr()) return false;
    while (match(TokenKind::OrOr)) {
        if (!parseLAndExpr()) return false;
    }
    return true;
}

bool Parser::parseLAndExpr() {
    if (!parseRelExpr()) return false;
    while (match(TokenKind::AndAnd)) {
        if (!parseRelExpr()) return false;
    }
    return true;
}

bool Parser::parseRelExpr() {
    if (!parseAddExpr()) return false;
    while (true) {
        if (match(TokenKind::Less) || match(TokenKind::Greater) || match(TokenKind::LessEq) || match(TokenKind::GreaterEq) || match(TokenKind::EqEq) || match(TokenKind::NotEq)) {
            if (!parseAddExpr()) return false;
        } else break;
    }
    return true;
}

bool Parser::parseAddExpr() {
    if (!parseMulExpr()) return false;
    while (match(TokenKind::Plus) || match(TokenKind::Minus)) {
        if (!parseMulExpr()) return false;
    }
    return true;
}

bool Parser::parseMulExpr() {
    if (!parseUnaryExpr()) return false;
    while (match(TokenKind::Star) || match(TokenKind::Slash) || match(TokenKind::Percent)) {
        if (!parseUnaryExpr()) return false;
    }
    return true;
}

bool Parser::parseUnaryExpr() {
    if (match(TokenKind::Plus) || match(TokenKind::Minus) || match(TokenKind::Not)) {
        return parseUnaryExpr();
    }
    return parsePrimaryExpr();
}

bool Parser::parsePrimaryExpr() {
    if (match(TokenKind::LParen)) {
        if (!parseExpr()) return false;
        if (!expect(TokenKind::RParen)) return false;
        return true;
    }
    if (match(TokenKind::Number)) return true;
    if (match(TokenKind::Identifier)) {
        if (match(TokenKind::LParen)) {
            if (!is(TokenKind::RParen)) {
                if (!parseExpr()) return false;
                while (match(TokenKind::Comma)) {
                    if (!parseExpr()) return false;
                }
            }
            if (!expect(TokenKind::RParen)) return false;
        }
        return true;
    }
    errorLines.push_back(peek().line);
    return false;
}


