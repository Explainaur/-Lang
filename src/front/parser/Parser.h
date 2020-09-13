//
// Created by dyf on 2020/9/13.
//

#ifndef _LANG_PARSER_H
#define _LANG_PARSER_H

#include <iostream>
#include "front/lexer/Token.h"
#include "front/lexer/lexer.h"
#include "front/define/AST.h"
#include "front/define/color.h"

namespace front {

    class Parser {
    private:
        Token curToken;
        Lexer *lexer;

        bool isLeftParentheses();

        bool isRightParentheses();

        bool isComma();

    public:
        explicit Parser(Lexer *lex) : lexer(lex) {}

        void nextToken() { curToken = lexer->nextToken(); }

        ASTPtr LogError(const std::string &info) {
            auto red = Color::Modifier(Color::Code::FG_RED);
            auto def = Color::Modifier(Color::Code::BG_DEFAULT);
            std::cerr << lexer->GetFilename() << ":"
                      << curToken.getLineNumber() << ":"
                      << curToken.getPosition() << ": "
                      << red << "error: " << def
                      << info << std::endl;
            return nullptr;
        }

        ASTPtr ParseNumber();

        ASTPtr ParseVariable();

        ASTPtr ParseExpression();

        ASTPtr ParseParenExpr();

        ASTPtr ParsePrimary();
    };

}

#endif //_LANG_PARSER_H
