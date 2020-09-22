//
// Created by dyf on 2020/9/13.
//

#ifndef _LANG_AST_H
#define _LANG_AST_H

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"


namespace front {
    class ExprAST;

    typedef std::shared_ptr<ExprAST> ASTPtr;

    class ExprAST {
    public:
        virtual llvm::Value *codegen() = 0;

        virtual ~ExprAST() = default;
    };

    class NumberAST : public ExprAST {
    private:
        double value;
    public:
        explicit NumberAST(double num) : value(num) {}

        llvm::Value *codegen() override;
    };

    class VariableAST : public ExprAST {
    private:
        std::string value;
    public:
        explicit VariableAST(std::string name) : value(std::move(name)) {}

        llvm::Value *codegen() override;

    };

    class UnaryAST : public ExprAST {
    private:
        std::string oper;
        ASTPtr operand;
    public:
        UnaryAST(std::string operator_, ASTPtr operand_) : oper(std::move(operator_)), operand(std::move(operand_)) {}

        llvm::Value *codegen() override;
    };

    class BinaryAST : public ExprAST {
    private:
        std::string op;
        ASTPtr LHS, RHS;
    public:
        BinaryAST(std::string oper, ASTPtr lhs, ASTPtr rhs) :
                op(std::move(oper)), LHS(std::move(lhs)), RHS(std::move(rhs)) {}

        llvm::Value *codegen() override;
    };

    class IfExprAST : public ExprAST {
        ASTPtr condition, then_, else_;
    public:
        IfExprAST(ASTPtr cond, ASTPtr then, ASTPtr _else) :
                condition(std::move(cond)), then_(std::move(then)), else_(std::move(_else)) {}

        llvm::Value *codegen() override;
    };

    class ForExprAST : public ExprAST {
        std::string valueName;
        ASTPtr start_, end_, step, body;
    public:
        ForExprAST(const std::string name, ASTPtr start, ASTPtr end, ASTPtr step, ASTPtr body) :
                valueName(name), start_(std::move(start)), end_(std::move(end)), step(std::move(step)),
                body(std::move(body)) {}

        llvm::Value *codegen() override;
    };

    class CallAST : public ExprAST {
    private:
        std::string symbol;
        std::vector<ASTPtr> args;
    public:
        CallAST(std::string symbol_, std::vector<ASTPtr> args_) :
                symbol(std::move(symbol_)), args(std::move(args_)) {}

        llvm::Value *codegen() override;
    };

    class ProtoTypeAST {
    private:
        std::string funcName;
        std::vector<std::string> args;
    public:

        ProtoTypeAST(std::string name, std::vector<std::string> args_) :
                funcName(std::move(name)), args(std::move(args_)) {}

        std::string getName() { return funcName; }

        llvm::Function *codegen();
    };

    typedef std::shared_ptr<ProtoTypeAST> ProtoPtr;

    class FunctionAST {
    private:
        ProtoPtr protoType;
        ASTPtr body;
    public:
        FunctionAST(ProtoPtr proto, ASTPtr body) :
                protoType(std::move(proto)), body(std::move(body)) {}

        llvm::Function *codegen();
    };

    typedef std::shared_ptr<FunctionAST> FuncPtr;
}

#endif //_LANG_AST_H
