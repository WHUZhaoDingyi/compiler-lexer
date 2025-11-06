#pragma once

#include <string>

enum class TokenKind {
    EndOfFile,
    Invalid,

    Identifier,
    Number,

    KwInt,
    KwVoid,
    KwIf,
    KwElse,
    KwWhile,
    KwBreak,
    KwContinue,
    KwReturn,

    LParen,
    RParen,
    LBrace,
    RBrace,
    Comma,
    Semicolon,
    Assign,

    OrOr,
    AndAnd,
    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    Less,
    Greater,
    LessEq,
    GreaterEq,
    EqEq,
    NotEq,

    Not,
};

struct Token {
    TokenKind kind {TokenKind::Invalid};
    std::string lexeme;
    int line {1};
};

inline const char* tokenKindName(TokenKind k) {
    switch (k) {
        case TokenKind::EndOfFile: return "EOF";
        case TokenKind::Invalid: return "Invalid";
        case TokenKind::Identifier: return "Identifier";
        case TokenKind::Number: return "Number";
        case TokenKind::KwInt: return "int";
        case TokenKind::KwVoid: return "void";
        case TokenKind::KwIf: return "if";
        case TokenKind::KwElse: return "else";
        case TokenKind::KwWhile: return "while";
        case TokenKind::KwBreak: return "break";
        case TokenKind::KwContinue: return "continue";
        case TokenKind::KwReturn: return "return";
        case TokenKind::LParen: return "(";
        case TokenKind::RParen: return ")";
        case TokenKind::LBrace: return "{";
        case TokenKind::RBrace: return "}";
        case TokenKind::Comma: return ",";
        case TokenKind::Semicolon: return ";";
        case TokenKind::Assign: return "=";
        case TokenKind::OrOr: return "||";
        case TokenKind::AndAnd: return "&&";
        case TokenKind::Plus: return "+";
        case TokenKind::Minus: return "-";
        case TokenKind::Star: return "*";
        case TokenKind::Slash: return "/";
        case TokenKind::Percent: return "%";
        case TokenKind::Less: return "<";
        case TokenKind::Greater: return ">";
        case TokenKind::LessEq: return "<=";
        case TokenKind::GreaterEq: return ">=";
        case TokenKind::EqEq: return "==";
        case TokenKind::NotEq: return "!=";
        case TokenKind::Not: return "!";
    }
    return "?";
}


