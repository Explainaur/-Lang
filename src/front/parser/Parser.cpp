//
// Created by dyf on 2020/9/13.
//

#include "Parser.h"

#include <map>

namespace front {

    static std::map<std::string, int> BinopPrecedence;


    bool Parser::isLeftParentheses() {
        if (curToken.getTokenType() == TokenType::Operator ||
            curToken.getOperValue() == "(") {
            return true;
        }
        return false;
    }

    bool Parser::isRightParentheses() {
        if (curToken.getTokenType() == TokenType::Operator ||
            curToken.getOperValue() == ")") {
            return true;
        }
        return false;
    }

    bool Parser::isComma() {
        if (curToken.getTokenType() == TokenType::Operator ||
            curToken.getOperValue() == ",") {
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
        if (curToken.getTokenType() != TokenType::Operator) {
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
            LogError("Unknow type when parsing expression.");
        }
    }


}