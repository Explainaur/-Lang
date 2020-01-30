#ifndef FRONT_PARSER_AST_H
#define FROEN_PARSER_AST_H


#include "../define/define.h"

class BaseAST {
    private:
        FactorType type = FactorType::Nil;

    public:
        virtual ~BaseAST() = default;
        virtual Valptr Eval() = 0;
        virtual FactorType type();
};

/*
 * StringAST: 
 *  T = string; type = FactorType::String
 * 
 * NumberAST:
 *  T = int; type = FactorType::Number
 * 
 * IdAST:
 *  T = string; type = FactorType::Identifier
 */
template <typename T>
class FactorAST : public BaseAST {
    private:
        T value;
        FactorType type;

    public:
        FactorAST(T val, FactorType type_)
            : value(val), type(type_) {}
        Valptr Eval() override;
        FactorType type() { return type; }
};


/*
 * id = "string"
 */
class AssignAST : public BaseAST {
    private:
        ASTptr id;
        ASTptr value;

    public:
        AssignAST(ASTptr id_, ASTptr val) 
            : id(std::move(id_)), value(std::move(val)) {}
        Valptr Eval() override;
};

/*
 *  expr op expr
 */
class ArithAST : public BaseAST {
    private:
        string op;
        ASTptr left;
        ASTptr right;

    public:
        ArithAST(string op_, ASTptr l, ASTptr r)
            : op(op_), left(move(l)), right(move(r)) {}
        Valptr Eval() override;
};

/* 
 * define function:
 *  def id([args_]) {{statement}} 
 */
class FuncDefAST : public BaseAST {
    private:
        string id;
        Args args_;
        Block statement;
    
    public:
        FuncDefAST(string id_, Args args, Block stat)
            : id(id_), args_(std::move(args)), statement(std::move(stat)) {}
        Valptr Eval() override;
};


/*
 * id([args_])
 */
class FuncAST : public BaseAST {
    private:
        string id;
        Args args_;

    public:
        FuncAST(string id_, Args args)
            : id(id_), args_(std::move(args_)) {}
        Valptr Eval() override;
};

#endif