//
// Created by dyf on 2020/9/13.
//

#include "Parser.h"
#include "mid/IRGenerator.h"

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
            ASTPtr varAst = std::make_shared<VariableAST>(identName);
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

    ProtoPtr Parser::ParseProtoType() {
        if (curToken.getTokenType() != TokenType::Identifier) {
            LogError("Expect function name in ProtoType.");
            return nullptr;
        }

        std::string funcName = curToken.getIdentValue();
        nextToken();

        if (!isLeftParentheses()) {
            LogError("Expect '(' in ProtoType.");
            return nullptr;
        }
        nextToken();    //eat '('

        std::vector<std::string> args;
        while (curToken.getTokenType() == TokenType::Identifier) {
            args.push_back(curToken.getIdentValue());
            nextToken();
            if (isRightParentheses()) break;
            if (!isComma()) {
                LogError("Expect ',' in ProtoType.");
                return nullptr;
            }
            nextToken();    // eat ','
        }
        if (!isRightParentheses()) {
            LogError("Expect ')' in ProtoType.");
            return nullptr;
        }
        nextToken();    // eat ')'

        return std::make_shared<ProtoTypeAST>(funcName, args);
    }

    FuncPtr Parser::ParseDefine() {
        nextToken();    // eat def
        auto protoType = ParseProtoType();
        if (!protoType) return nullptr;

        if (auto expr = ParseExpression()) {
            return std::make_shared<FunctionAST>(protoType, expr);
        }
        return nullptr;
    }

    ProtoPtr Parser::ParseExtern() {
        nextToken();    // eat extern
        return ParseProtoType();
    }

    FuncPtr Parser::ParseTopLevelExpr() {
        if (auto expr = ParseExpression()) {
            // make anonymous proto
            auto proto = std::make_shared<ProtoTypeAST>("", std::vector<std::string>());
            return std::make_shared<FunctionAST>(proto, expr);
        }
        return nullptr;
    }

    void Parser::HandleDefinition() {
        if (auto FnAST = ParseDefine()) {
            if (auto *FnIR = FnAST->codegen()) {
//                FnIR->print(llvm::errs());
                return;
            }
        } else {
            // Skip token for error recovery.
            nextToken();
        }
    }

    void Parser::HandleExtern() {
        if (auto ProtoAST = ParseExtern()) {
            if (auto *FnIR = ProtoAST->codegen()) {
//                FnIR->print(llvm::errs());
                return;
            }
        } else {
            // Skip token for error recovery.
            nextToken();
        }
    }

    void Parser::HandleTopLevelExpression() {
        // Evaluate a top-level expression into an anonymous function.
        if (auto FnAST = ParseTopLevelExpr()) {
            if (auto *FnIR = FnAST->codegen()) {
//                FnIR->print(llvm::errs());
                return;
            }
        } else {
            // Skip token for error recovery.
            nextToken();
        }
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
                HandleDefinition();
                LOGINFO("Parsed a function definition.");
            } else if (isExtern()) {
                HandleExtern();
                LOGINFO("Parsed an extern function.");
            } else {
                HandleTopLevelExpression();
                LOGINFO("Parsed a top level expression.");
            }
        }
    }


}