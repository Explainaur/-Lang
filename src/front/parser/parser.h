#ifndef FRONT_PARSER_H
#define FRONT_PARSER_H

#include "../lexer/lexer.h"
#include "ast.h"

class Parser {
    private:
        Lexer lexer;
        Token token;
        Token prev;

    public:
        Parser(Lexer &lexer_) : lexer(lexer_) {}
        bool match(char);
        bool match(string);
        bool match(const char *);
        
        void nextToken() { token = lexer.nextToken(); }
        
        ASTptr parseExpr();
        ASTptr parseAssign(Token id);
        ASTptr parseDefine();
        ASTptr parseIf();
        ASTptr parseWhile();
        ASTptr parseFunc();
        ASTptr parseTerm();
        ASTptr parseFactor();

        bool isFactor(ASTptr);
        bool isTermSameType(ASTptr, ASTptr);
};

#endif