#include <iostream>
#include "front/lexer/lexer.h"
#include "front/parser/parser.h"
#include <fstream>
using namespace std;
int main() {
    fstream fd;
    fd.open("../test/a.x", ios::in);
    Token token;
    Lexer lexer = Lexer(fd);
    Parser parser = Parser(lexer);

    // for (int i=0; i<10;i++) {
        // token = lexer.nextToken();
        // cout << "Type is " << token.type2Str() << " Value is " << token.val() << endl;
    // }

    parser.nextToken();    
    for (int i=0;i<5;i++) {
        ASTptr tree = parser.parseExpr();
        if (tree.get()) {
            cout << "--- main ---" << endl;
            tree->print();
            cout <<"--- ---- ---" << endl;
        }
    }

    return 0;
}