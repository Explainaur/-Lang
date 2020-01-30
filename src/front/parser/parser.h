#ifndef FRONT_PARSER_H
#define FRONT_PARSER_H

#include "../lexer/lexer.h"
#include "ast.h"

class Parser {
    private:
        Lexer lexer;
        Token token;

    public:
        Parser(Lexer &lexer_) : lexer(lexer_) {}
        bool match(char);
        Token nextToken() { token = lexer.nextToken(); }
        ASTptr parseExpr();
        ASTptr parseAssign(Token id);
        ASTptr parseDefine();
        ASTptr parseIf();
        ASTptr parseWhile();
        ASTptr parseFunc();
        ASTptr parseTerm();
        ASTptr parseFactor();
};

#endif