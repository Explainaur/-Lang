//
// Created by dyf on 2020/9/13.
//

#ifndef _LANG_AST_H
#define _LANG_AST_H

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace front {
    class BaseAST;

    typedef std::shared_ptr<BaseAST> ASTPtr;

    class BaseAST {
    };

    class ExprAST : public BaseAST {
    };

    class NumberAST : public ExprAST {
    private:
        double value;
    public:
        explicit NumberAST(double num) : value(num) {}

        int getValue() { return value; }
    };

    class VariableAST : public ExprAST {
    private:
        std::string value;
    public:
        explicit VariableAST(std::string name) : value(std::move(name)) {}
    };

    class BinaryAST : public ExprAST {
    private:
        std::string op;
        ASTPtr LHS, RHS;
    public:
        BinaryAST(std::string oper, ASTPtr lhs, ASTPtr rhs) :
                op(std::move(oper)), LHS(std::move(lhs)), RHS(std::move(rhs)) {}
    };

    class CallAST : public ExprAST {
    private:
        std::string symbol;
        std::vector<ASTPtr> args;
    public:
        CallAST(std::string symbol_, std::vector<ASTPtr> args_) :
        symbol(std::move(symbol_)), args(std::move(args_)){}
    };

    class ProtoTypeAST : public BaseAST {
    private:
        std::string funcName;
        std::vector<std::string> args;
    public:
        ProtoTypeAST(std::string name, std::vector<std::string> args_) :
                funcName(std::move(name)), args(std::move(args_)) {}
    };

    class FunctionAST : BaseAST {
    private:
        ASTPtr protoType;
        ASTPtr body;
    public:
        FunctionAST(ASTPtr proto, ASTPtr body) :
                protoType(std::move(proto)), body(std::move(body)) {}
    };
}

#endif //_LANG_AST_H
