#include <iostream>
#include "front/lexer/lexer.h"

using namespace Front;

int main(int argc, char* argv[]) {
    auto *lexer = new Lexer();
    lexer->openFile(argv[1]);
    for (int i = 0; i < 20 ; i++) {
        lexer->nextToken();
    }
    return 0;
}
