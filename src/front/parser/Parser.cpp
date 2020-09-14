//
// Created by dyf on 2020/9/13.
//

#include "Parser.h"

#include <map>

namespace front {


    void Parser::setPrecedence() {
        BinopPrecedence["<"] = 10;
        BinopPrecedence[">"] = 10;
        BinopPrecedence["+"] = 20;
        BinopPrecedence["-"] = 20;
        BinopPrecedence["*"] = 30;
        BinopPrecedence["/"] = 30;
    }

    int Parser::getPrecedence() {
        if (curToken.getTokenType() != TokenType::Operator) return -1;
        int prec = BinopPrecedence[curToken.getOperValue()];
        if (prec <= 0) return -1;
        return prec;
    }

    bool Parser::isLeftParentheses() {
        if (curToken.getTokenType() == TokenType::Operator &&
            curToken.getOperValue() == "(") {
            return true;
        }
        return false;
    }

    bool Parser::isRightParentheses() {
        if (curToken.getTokenType() == TokenType::Operator &&
            curToken.getOperValue() == ")") {
            return true;
        }
        return false;
    }

    bool Parser::isComma() {
        if (curToken.getTokenType() == TokenType::Operator &&
            curToken.getOperValue() == ",") {
            return true;
        }
        return false;
    }

    bool Parser::isDefine() {
        if (curToken.getTokenType() == TokenType::Keyword &&
            curToken.getKeywordValue() == "def") {
            return true;
        }
        return false;
    }

    bool Parser::isExtern() {
        if (curToken.getTokenType() == TokenType::Keyword &&
            curToken.getKeywordValue() == "extern") {
            return true;
        }
        return false;
    }

    // Parse the number
    ASTPtr Parser::ParseNumber() {
        assert(curToken.getTokenType() == TokenType::Double);
        ASTPtr numAst = std::make_shared<NumberAST>(curToken.getNumValue());
        nextToken();    // forward one step
        return numAst;
    }

    // Parse the variable or function
    ASTPtr Parser::ParseVariable() {
        assert(curToken.getTokenType() == TokenType::Identifier);
        std::string identName = curToken.getIdentValue();

        nextToken();

        // not '(' -> not function call
        if (!isLeftParentheses()) {
            ASTPtr varAst = std::make_shared<VariableAST>(curToken.getIdentValue());
            return varAst;
        }

        assert(isLeftParentheses());

        nextToken();    // eat '('
        std::vector<ASTPtr> args;
        if (!isRightParentheses()) {

            while (true) {
                if (ASTPtr arg = ParseExpression()) {
                    args.push_back(std::move(arg));
                } else {
                    return nullptr;
                }

                if (isRightParentheses()) {
                    break;
                }

                if (!isComma()) {
                    return LogError("Expect a ',' or ')' in arguments list.");
                }
                nextToken();
            }
        }
        nextToken();    // eat ')'
        return std::make_shared<CallAST>(identName, args);
    }

    ASTPtr Parser::ParseParenExpr() {
        nextToken();    // eat '('
        auto expr = ParseExpression();
        if (expr == nullptr) {
            return nullptr;
        }

        if (!isRightParentheses()) {
            return LogError("Expected ')'.");
        }
        nextToken();    // eat ')'
        return expr;
    }

    ASTPtr Parser::ParsePrimary() {
        if (curToken.getTokenType() == TokenType::Identifier) {
            return ParseVariable();
        } else if (curToken.getTokenType() == TokenType::Double) {
            return ParseNumber();
        } else if (isLeftParentheses()) {
            return ParseParenExpr();
        } else {
            return LogError("Unknow type when parsing expression.");
        }
    }

    ASTPtr Parser::ParseExpression() {
        auto LHS = ParsePrimary();
        if (LHS == nullptr) return nullptr;
        return ParseBinaryOPRHS(0, LHS);
    }

    ASTPtr Parser::ParseBinaryOPRHS(int prec, ASTPtr lhs) {
        while (true) {
            int tokPrec = getPrecedence();

            if (tokPrec < prec) return lhs;

            auto binaryOp = curToken;
            nextToken();

            auto rhs = ParsePrimary();
            if (!rhs) return nullptr;

            int nextPrec = getPrecedence();
            if (tokPrec < nextPrec) {
                rhs = ParseBinaryOPRHS(tokPrec + 1, std::move(rhs));
                if (!rhs) return nullptr;
            }

            lhs = std::make_shared<BinaryAST>(
                    binaryOp.getOperValue(), std::move(lhs), std::move(rhs));
        }
    }

    ASTPtr Parser::ParseProtoType() {
        if (curToken.getTokenType() != TokenType::Identifier) {
            return LogError("Expect function name in ProtoType.");
        }

        std::string funcName = curToken.getIdentValue();
        nextToken();

        if (!isLeftParentheses()) {
            return LogError("Expect '(' in ProtoType.");
        }
        nextToken();    //eat '('

        std::vector<std::string> args;
        while (curToken.getTokenType() == TokenType::Identifier) {
            args.push_back(curToken.getIdentValue());
            nextToken();
            if (isRightParentheses()) break;
            if (!isComma()) {
                return LogError("Expect ',' in ProtoType.");
            }
            nextToken();    // eat ','
        }
        if (!isRightParentheses()) {
            return LogError("Expect ')' in ProtoType.");
        }
        nextToken();    // eat ')'

        return std::make_shared<ProtoTypeAST>(funcName, args);
    }

    ASTPtr Parser::ParseDefine() {
        nextToken();    // eat def
        auto protoType = ParseProtoType();
        if (!protoType) return nullptr;

        if (auto expr = ParseExpression()) {
            return std::make_shared<FunctionAST>(protoType, expr);
        }
        return nullptr;
    }

    ASTPtr Parser::ParseExtern() {
        nextToken();    // eat extern
        return ParseProtoType();
    }

    ASTPtr Parser::ParseTopLevelExpr() {
        if (auto expr = ParseExpression()) {
            // make anonymous proto
            auto proto = std::make_shared<ProtoTypeAST>("", std::vector<std::string>());
            return std::make_shared<FunctionAST>(proto, expr);
        }
        return nullptr;
    }

    void Parser::Loop() {
        nextToken();
        while (true) {
            if (curToken.getTokenType() == TokenType::Eof) {
                return;
            } else if (curToken.getTokenType() == TokenType::Operator ||
                       curToken.getOperValue() == ";") {
                nextToken();
            } else if (isDefine()) {
                ParseDefine();
                LOGINFO("Parsed a function definition.");
            } else if (isExtern()) {
                ParseExtern();
                LOGINFO("Parsed an extern function.");
            } else {
                ParseTopLevelExpr();
                LOGINFO("Parsed a top level expression.");
            }
        }
    }


}