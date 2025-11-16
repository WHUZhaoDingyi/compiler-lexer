#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "lexer.h"
#include "parser.h"

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string src, line;
    while (std::getline(std::cin, line)) {
        src += line;
        src.push_back('\n');
    }

    // 词法分析
    Scanner scanner(src);
    std::vector<Token> tokens;
    while (true) {
        Token t = scanner.nextToken();
        tokens.push_back(t);
        if (t.kind == Tag::End) break;
    }

    // 语法分析
    Parser parser(tokens);
    bool ok = parser.parse(); // ok 用不到也没关系，逻辑在错误表里

    // 合并词法 + 语法错误
    std::map<int, std::string> allErrors = scanner.getErrors();
    const auto &parseErr = parser.getErrors();
    for (const auto &kv : parseErr) {
        if (allErrors.find(kv.first) == allErrors.end()) {
            allErrors[kv.first] = kv.second;
        }
    }

    if (allErrors.empty()) {
        std::cout << "accept\n";
    } else {
        std::cout << "reject\n";
        for (const auto &kv : allErrors) {
            if (kv.second.empty())
                std::cout << kv.first << "\n";
            else
                std::cout << kv.first << ' ' << kv.second << "\n";
        }
    }

    return 0;
}
