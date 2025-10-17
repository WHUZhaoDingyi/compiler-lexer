#include "lexer.h"
#include <iostream>
#include <string>

using namespace std;

int main() 
{
    string input;
    string line;
    
    while (getline(cin, line)) 
    {
        input += line + "\n";
    }
    
    Lexer lexer(input);
    lexer.tokenize();
    lexer.printTokens();
    
    return 0;
}
