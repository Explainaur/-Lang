#include <iostream>
#include "front/lexer/lexer.h"
#include "front/parser/Parser.h"

using namespace front;

int main(int argc, char* argv[]) {
    auto *lexer = new Lexer();
    lexer->openFile(argv[1]);
//    for (auto token = lexer->nextToken(); token.getTokenType() != TokenType::Eof; token = lexer->nextToken());
    auto *parser = new Parser(lexer);
    parser->Loop();

    return 0;
}
