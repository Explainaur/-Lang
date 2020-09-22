//
// Created by dyf on 2020/9/13.
//

#ifndef _LANG_PARSER_H
#define _LANG_PARSER_H

#include <map>
#include <iostream>
#include "front/lexer/Token.h"
#include "front/lexer/lexer.h"
#include "front/define/AST.h"
#include "front/define/color.h"

namespace front {

#define LOGINFO(info) \
    std::cerr << __FILE__ << ":" << __LINE__ << ":";Parser::LogInfo(info)

    class Parser {
    private:
        std::map<std::string, int> BinopPrecedence;
        Token curToken;
        Lexer *lexer;

        bool isLeftParentheses();

        bool isRightParentheses();

        bool isComma();

        bool isEqualSign();

        bool isDefine();

        bool isExtern();

        bool isIf();

        bool isThen();

        bool isElse();

        bool isFor();

        bool isIn();

        bool isUnary();

        void setPrecedence();

        int getPrecedence();

    public:
        explicit Parser(Lexer *lex) : lexer(lex) {
            setPrecedence();
        }

        void nextToken() { curToken = lexer->nextToken(); }

        ASTPtr LogError(const std::string &info) {
            auto red = Color::Modifier(Color::Code::FG_RED);
            auto def = Color::Modifier(Color::Code::FG_DEFAULT);
            std::cerr << lexer->GetFilename() << ":"
                      << curToken.getLineNumber() << ":"
                      << curToken.getPosition() << ": "
                      << red << "error: " << def
                      << info << std::endl;
            return nullptr;
        }

        static ASTPtr LogInfo(const std::string &info) {
            auto green = Color::Modifier(Color::Code::FG_GREEN);
            auto def = Color::Modifier(Color::Code::FG_DEFAULT);
            std::cerr << green << "info: " << def
                      << info << std::endl;
            return nullptr;
        }

        ASTPtr ParseNumber();

        ASTPtr ParseVariable();

        ASTPtr ParseExpression();

        ASTPtr ParseParenExpr();

        ASTPtr ParsePrimary();

        ASTPtr ParseIfExpr();

        ASTPtr ParseForExpr();

        ASTPtr ParseUnary();

        ASTPtr ParseBinaryOPRHS(int prec, ASTPtr lhs);

        ProtoPtr ParseProtoType();

        ProtoPtr ParseExtern();

        FuncPtr ParseDefine();

        FuncPtr ParseTopLevelExpr();

        void Loop();

        void HandleDefinition();

        void HandleExtern();

        void HandleTopLevelExpression();

    };

}

#endif //_LANG_PARSER_H
