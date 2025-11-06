#include <bits/stdc++.h>
#include "lexer.h"
#include "parser.h"

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string input;
    {
        std::ostringstream oss;
        oss << std::cin.rdbuf();
        input = oss.str();
    }

    std::vector<int> errorLines;
    std::vector<std::string> errorMsgs;

    Lexer lex(std::move(input));
    std::vector<Token> tokens = lex.lexAll(errorLines, errorMsgs);

    Parser parser(tokens, errorLines);
    bool ok = parser.parse();

    if (ok && errorLines.empty()) {
        std::cout << "accept\n";
    } else {
        std::cout << "reject\n";
        for (int ln : errorLines) {
            std::cout << ln << "\n";
        }
    }
    return 0;
}


