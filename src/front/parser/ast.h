#ifndef FRONT_PARSER_AST_H
#define FROEN_PARSER_AST_H


#include "../define/define.h"

class BaseAST {
    public:
        virtual ~BaseAST() = default;
        virtual Valptr Eval() = 0;
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
        Valptr Eval() override;
};

/* 
 * Arithmetic and Assignment and while (left_arg) { right_arg }: 
 *  T = string;
 * if (op) { left_arg } else { right_arg }:
 *  T = ASTptr;
 */
template <typename T>
class StateAST : public BaseAST {
    private:
        ASTptr left_arg, right_arg;
        T op;

    public:
        Valptr Eval() override;
};

/* 
 * define function:
 *  def id([args_]) {{statement}} 
 */
class define : public BaseAST {
    private:
        string id;
        Args args_;
        ASTptr statement;
    
    public:
};

#endif