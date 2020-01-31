#include "src/front/parser/ast.h"
#include "src/util/error.h"

namespace {
    string Type2Str(FactorType type) {
        switch (type) {
            case FactorType::FArithmetic : return "FArithmetic";
            case FactorType::FIdentifier : return "FIdentifier";
            case FactorType::FNumber : return "FNumber";
            case FactorType::FString : return "FString";
            case FactorType::FunCall : return "FunCall"; 
        }
        return "Nil";
    }
}

// Eval
Valptr FactorAST::Eval() { return std::make_shared<Value>(); }

Valptr AssignAST::Eval() { return std::make_shared<Value>(); }

Valptr ArithAST::Eval() { return std::make_shared<Value>(); }

Valptr FuncDefAST::Eval() { return std::make_shared<Value>(); }

Valptr FuncAST::Eval() { return std::make_shared<Value>(); }

// clone
ASTptr BaseAST::clone() {
    return std::make_unique<BaseAST>();
}

ASTptr FactorAST::clone() {
    return std::make_unique<FactorAST>(type_);
}

ASTptr NumAST::clone() {
    return std::make_unique<NumAST>(value_, FactorType::FNumber);
}

ASTptr StringAST::clone() {
    return std::make_unique<StringAST>(value_, FactorType::FString);
}

ASTptr IdAST::clone() {
    return std::make_unique<IdAST>(id, FactorType::FIdentifier);
}

ASTptr AssignAST::clone() {
    return std::make_unique<AssignAST>(id->clone(), value->clone());
}

ASTptr ArithAST::clone() {
    return std::make_unique<ArithAST>(op, left->clone(), right->clone(),
        FactorType::FArithmetic);
}

ASTptr FuncAST::clone() { 
    return std::make_unique<FuncAST>(id, args_); 
}

ASTptr FuncDefAST::clone() {
    Block stat;
    int l = 0;
    for(auto i = statement[l]->clone();l<statement.size();l++) {
        i = statement[l]->clone();
        stat.push_back(move(i));
    }
    return std::make_unique<FuncDefAST>(id, args_, move(stat));
}

// print info
void NumAST::print() {
    log_msg("NumAST value is %d", this->value_);
}

void StringAST::print() {
    log_msg("StringAST value is %s", this->value_.c_str());
}

void IdAST::print() {
    log_msg("IdAST id is %s", this->id.c_str());
}

void AssignAST::print() {
    if (this->value->type() == FactorType::FNumber)
        log_msg("AssignAST id = %s value = %d",
            this->id->svalue().c_str(), this->value->ivalue());
    else
        log_msg("AssignAST id = %s value = %s",
            this->id->svalue().c_str(), this->value->svalue().c_str()); 
}

void ArithAST::print() {
    if (!left.get())
        err_quit("not got left");
    if (!right.get())
        err_quit("not got right");

        if (left->type() == FactorType::FNumber && 
        right->type() == FactorType::FNumber) {
        log_msg("ArithAST, op is '%s', left = %d right = %d",
            op.c_str(), left->ivalue(), right->ivalue());
    }
    else if (left->type() == FactorType::FString &&
             (right->type() == FactorType::FString ||
              right->type() == FactorType::FIdentifier ||
              right->type() == FactorType::FunCall)) {
        log_msg("ArithAST, op is '%s', left = %s right = %s",
            op.c_str(), left->svalue().c_str(), right->svalue().c_str());
    }
    else if (left->type() == FactorType::FNumber &&
             right->type() == FactorType::FArithmetic) {
        log_msg("ArithAST, op is '%s', left = %d right is expr which op is %s",
            op.c_str(), left->ivalue(), right->svalue().c_str());
    }

}