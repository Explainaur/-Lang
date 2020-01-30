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

    
    return 0;
}