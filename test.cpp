#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <map>

using namespace std;

/*------------  词法部分  ------------*/

enum class Tag {
    End,
    KwInt, KwVoid, KwIf, KwElse, KwWhile,
    KwBreak, KwContinue, KwReturn,
    Id, Number,
    Plus, Minus, Star, Slash, Percent,
    Lt, Le, Gt, Ge, Eq, Ne,
    AndAnd, OrOr, Not,
    Assign,
    LParen, RParen, LBrace, RBrace,
    Semicolon, Comma
};

struct Token {
    Tag kind;
    string lexeme;
    int line;
};

class Scanner {
public:
    explicit Scanner(const string &src)
        : text(src), p(0), line(1) {}

    Token nextToken() {
        skipSpaceAndComments();

        Token tok;
        tok.lexeme.clear();
        tok.line = line;

        char c = peek();
        if (c == '\0') {
            tok.kind = Tag::End;
            return tok;
        }

        // 标识符 / 关键字
        if (isalpha(c) || c == '_') {
            string id;
            while (isalnum(peek()) || peek() == '_') {
                id.push_back(get());
            }
            tok.lexeme = id;
            if (id == "int") tok.kind = Tag::KwInt;
            else if (id == "void") tok.kind = Tag::KwVoid;
            else if (id == "if") tok.kind = Tag::KwIf;
            else if (id == "else") tok.kind = Tag::KwElse;
            else if (id == "while") tok.kind = Tag::KwWhile;
            else if (id == "break") tok.kind = Tag::KwBreak;
            else if (id == "continue") tok.kind = Tag::KwContinue;
            else if (id == "return") tok.kind = Tag::KwReturn;
            else tok.kind = Tag::Id;
            return tok;
        }

        // 数字（这里只按整数处理，负号作为一元运算符）
        if (isdigit(c)) {
            string num;
            while (isdigit(peek())) {
                num.push_back(get());
            }
            tok.kind = Tag::Number;
            tok.lexeme = num;
            return tok;
        }

        // 符号类
        switch (c) {
            case '+':
                get();
                tok.kind = Tag::Plus;
                return tok;
            case '-':
                get();
                tok.kind = Tag::Minus;
                return tok;
            case '*':
                get();
                tok.kind = Tag::Star;
                return tok;
            case '/':
                get();
                tok.kind = Tag::Slash;
                return tok;
            case '%':
                get();
                tok.kind = Tag::Percent;
                return tok;
            case '(':
                get();
                tok.kind = Tag::LParen;
                return tok;
            case ')':
                get();
                tok.kind = Tag::RParen;
                return tok;
            case '{':
                get();
                tok.kind = Tag::LBrace;
                return tok;
            case '}':
                get();
                tok.kind = Tag::RBrace;
                return tok;
            case ';':
                get();
                tok.kind = Tag::Semicolon;
                return tok;
            case ',':
                get();
                tok.kind = Tag::Comma;
                return tok;
            case '<':
                get();
                if (peek() == '=') {
                    get();
                    tok.kind = Tag::Le;
                } else {
                    tok.kind = Tag::Lt;
                }
                return tok;
            case '>':
                get();
                if (peek() == '=') {
                    get();
                    tok.kind = Tag::Ge;
                } else {
                    tok.kind = Tag::Gt;
                }
                return tok;
            case '=':
                get();
                if (peek() == '=') {
                    get();
                    tok.kind = Tag::Eq;
                } else {
                    tok.kind = Tag::Assign;
                }
                return tok;
            case '!':
                get();
                if (peek() == '=') {
                    get();
                    tok.kind = Tag::Ne;
                } else {
                    tok.kind = Tag::Not;
                }
                return tok;
            case '&':
                get();
                if (peek() == '&') {
                    get();
                    tok.kind = Tag::AndAnd;
                    return tok;
                }
                break;
            case '|':
                get();
                if (peek() == '|') {
                    get();
                    tok.kind = Tag::OrOr;
                    return tok;
                }
                break;
            default:
                break;
        }

        // 未知字符：记录一条错误，然后跳过它，继续取下一个 token
        {
            int errLine = line;
            if (lexErrors.find(errLine) == lexErrors.end()) {
                lexErrors[errLine] = "Invalid character";
            }
            get(); // 丢弃该字符
        }
        return nextToken();
    }

    const map<int, string> &getErrors() const {
        return lexErrors;
    }

private:
    string text;
    size_t p;
    int line;
    map<int, string> lexErrors;

    char peek(int k = 0) const {
        if (p + k >= text.size()) return '\0';
        return text[p + k];
    }

    char get() {
        if (p >= text.size()) return '\0';
        char c = text[p++];
        if (c == '\n') ++line;
        return c;
    }

    void skipSpaces() {
        while (isspace(peek())) get();
    }

    bool skipCommentOnce() {
        // 单行注释 //
        if (peek() == '/' && peek(1) == '/') {
            while (peek() != '\n' && peek() != '\0') get();
            return true;
        }
        // 多行注释 /* ... */
        if (peek() == '/' && peek(1) == '*') {
            int startLine = line;
            get(); // '/'
            get(); // '*'
            while (true) {
                if (peek() == '\0') {
                    if (lexErrors.find(startLine) == lexErrors.end()) {
                        lexErrors[startLine] = "Unterminated comment";
                    }
                    return false;
                }
                if (peek() == '*' && peek(1) == '/') {
                    get(); // '*'
                    get(); // '/'
                    break;
                }
                get();
            }
            return true;
        }
        return false;
    }

    void skipSpaceAndComments() {
        while (true) {
            skipSpaces();
            if (!skipCommentOnce()) break;
        }
    }
};

/*------------  语法分析部分  ------------*/

class Parser {
public:
    explicit Parser(const vector<Token> &ts)
        : tokens(ts), idx(0), loopDepth(0), hasError(false) {}

    bool parse() {
        parseCompUnit();
        return !hasError;
    }

    const map<int, string> &getErrors() const {
        return parseErrors;
    }

private:
    vector<Token> tokens;
    size_t idx;
    int loopDepth;
    bool hasError;
    map<int, string> parseErrors;

    const Token &current(int offset = 0) const {
        size_t pos = idx + offset;
        if (pos >= tokens.size()) return tokens.back();
        return tokens[pos];
    }

    bool is(Tag k, int offset = 0) const {
        return current(offset).kind == k;
    }

    void advance() {
        if (idx < tokens.size()) ++idx;
    }

    void recordError(const string &msg) {
        hasError = true;
        int ln = current().line;
        if (parseErrors.find(ln) == parseErrors.end()) {
            parseErrors[ln] = msg;
        }
    }

    bool accept(Tag k) {
        if (is(k)) {
            advance();
            return true;
        }
        return false;
    }

    bool expect(Tag k, const string &msg) {
        if (is(k)) {
            advance();
            return true;
        }
        recordError(msg);
        return false;
    }

    void sync() {
        while (!is(Tag::End) && !is(Tag::Semicolon) && !is(Tag::RBrace)) {
            advance();
        }
        if (is(Tag::Semicolon)) advance();
    }

    /*  编译单元：FuncDef+  */
    void parseCompUnit() {
        while (!is(Tag::End)) {
            parseFuncDef();
        }
    }

    /*  FuncDef → ("int" | "void") ID "(" (Param ("," Param)*)? ")" Block  */
    void parseFuncDef() {
        if (!is(Tag::KwInt) && !is(Tag::KwVoid)) {
            recordError("Expected function return type");
            sync();
            if (is(Tag::RBrace)) advance();
            return;
        }
        advance(); // 吃掉返回类型

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
    void parseParam() {
        expect(Tag::KwInt, "Expected int");
        expect(Tag::Id, "Expected identifier");
    }

    /* Block → "{" Stmt* "}" */
    void parseBlock() {
        if (!expect(Tag::LBrace, "Lack of '{'")) return;

        while (!is(Tag::RBrace) && !is(Tag::End)) {
            parseStmt();
        }

        expect(Tag::RBrace, "Lack of '}'");
    }

    /*  语句  */
    void parseStmt() {
        // 声明语句
        if (is(Tag::KwInt)) {
            advance(); // int
            expect(Tag::Id, "Expected identifier");
            if (accept(Tag::Assign)) {
                parseExpr();
            }
            while (accept(Tag::Comma)) {
                expect(Tag::Id, "Expected identifier");
                if (accept(Tag::Assign)) {
                    parseExpr();
                }
            }
            expect(Tag::Semicolon, "Lack of ';'");
            return;
        }

        // if 语句
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

        // while 语句
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

        // 以标识符开头：赋值 / 函数调用 / 错误
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
                // 比如：单独一个 ID; 这里按缺少分号处理
                expect(Tag::Semicolon, "Lack of ';'");
            }
            return;
        }

        // 空语句
        if (is(Tag::Semicolon)) {
            advance();
            return;
        }

        // 其他任何东西都视作意外 token
        recordError("Unexpected token");
        advance();
    }

    /* Expr → LOrExpr */
    void parseExpr() {
        parseLOrExpr();
    }

    /* LOrExpr → LAndExpr | LOrExpr "||" LAndExpr */
    void parseLOrExpr() {
        parseLAndExpr();
        while (is(Tag::OrOr)) {
            advance();
            parseLAndExpr();
        }
    }

    /* LAndExpr → RelExpr | LAndExpr "&&" RelExpr */
    void parseLAndExpr() {
        parseRelExpr();
        while (is(Tag::AndAnd)) {
            advance();
            parseRelExpr();
        }
    }

    /* RelExpr → AddExpr | RelExpr ( < > <= >= == != ) AddExpr */
    void parseRelExpr() {
        parseAddExpr();
        while (is(Tag::Lt) || is(Tag::Le) || is(Tag::Gt) || is(Tag::Ge)
               || is(Tag::Eq) || is(Tag::Ne)) {
            advance();
            parseAddExpr();
        }
    }

    /* AddExpr → MulExpr | AddExpr ("+" | "-") MulExpr */
    void parseAddExpr() {
        parseMulExpr();
        while (is(Tag::Plus) || is(Tag::Minus)) {
            advance();
            parseMulExpr();
        }
    }

    /* MulExpr → UnaryExpr | MulExpr ("*" | "/" | "%") UnaryExpr */
    void parseMulExpr() {
        parseUnaryExpr();
        while (is(Tag::Star) || is(Tag::Slash) || is(Tag::Percent)) {
            advance();
            parseUnaryExpr();
        }
    }

    /* UnaryExpr → PrimaryExpr | ("+" | "-" | "!") UnaryExpr */
    void parseUnaryExpr() {
        if (is(Tag::Plus) || is(Tag::Minus) || is(Tag::Not)) {
            advance();
            parseUnaryExpr();
        } else {
            parsePrimaryExpr();
        }
    }

    /* PrimaryExpr → ID | NUMBER | "(" Expr ")" | ID "(" (Expr ("," Expr)*)? ")" */
    void parsePrimaryExpr() {
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
            // 例如 z = z - ; 里的 “-” 后面缺少表达式，就会走到这里
            recordError("Expected expression");
            if (!is(Tag::End) && !is(Tag::Semicolon)) {
                advance();
            }
        }
    }
};

/*------------  主程序  ------------*/

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string src, line;
    while (getline(cin, line)) {
        src += line;
        src.push_back('\n');
    }

    // 词法分析
    Scanner scanner(src);
    vector<Token> tokens;
    while (true) {
        Token t = scanner.nextToken();
        tokens.push_back(t);
        if (t.kind == Tag::End) break;
    }

    // 语法分析
    Parser parser(tokens);
    bool ok = parser.parse();

    // 合并词法和语法错误（同一行只保留第一条）
    map<int, string> allErrors = scanner.getErrors();
    const auto &parseErr = parser.getErrors();
    for (const auto &kv : parseErr) {
        if (allErrors.find(kv.first) == allErrors.end()) {
            allErrors[kv.first] = kv.second;
        }
    }

    if (allErrors.empty()) {
        cout << "accept\n";
    } else {
        cout << "reject\n";
        for (const auto &kv : allErrors) {
            if (kv.second.empty())
                cout << kv.first << "\n";
            else
                cout << kv.first << " " << kv.second << "\n";
        }
    }

    return 0;
}
