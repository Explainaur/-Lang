#ifndef FRONT_PARSER_AST_H
#define FROEN_PARSER_AST_H


#include "../define/define.h"

class BaseAST {
    private:
        FactorType type_ = FactorType::Nil;

    public:
        BaseAST() {}
        BaseAST(FactorType type) : type_(type) {}
        virtual ~BaseAST() {}
        virtual Valptr Eval() { return Valptr(); }
        virtual FactorType type() { return type_; }
        virtual string svalue() { return string(""); }
        virtual int ivalue() { return 0; }
        virtual void print() {}
        virtual ASTptr clone();
};


class FactorAST : public BaseAST {
    private:
        FactorType type_;

    public:
        FactorAST(FactorType type) : type_(type) {}
        virtual Valptr Eval() override;
        virtual FactorType type() { return type_; }
        virtual ASTptr clone();
};

class NumAST : public FactorAST {
    private:
        int value_;

    public:
        NumAST(int val, FactorType type)
            : value_(val), FactorAST(type) {}
        int ivalue() { return value_; }
        ASTptr clone();
        void print();
};

class StringAST : public FactorAST {
    private:
        string value_;

    public:
        StringAST(string val, FactorType type)
            : value_(val), FactorAST(type) {}
        string svalue() { return value_; }
        ASTptr clone();
        void print();
};

class IdAST : public FactorAST {
    private:
        string id;

    public:
        IdAST(string id_, FactorType type)
            : id(id_), FactorAST(type) {}
        string svalue() { return id; }
        ASTptr clone();
        void print();
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
        ASTptr clone();
        void print();
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
        ArithAST(string op_, ASTptr l, ASTptr r, FactorType type)
            : op(op_), left(move(l)), right(move(r)), BaseAST(type) {}
        Valptr Eval() override;
        string svalue() { return op; }
        ASTptr clone();
        void print();
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
        string svalue() { return id; }
        ASTptr clone();
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
        ASTptr clone();
};

#endif