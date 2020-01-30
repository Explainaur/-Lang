#include "src/front/parser/ast.h"

Valptr FactorAST::Eval() { return std::make_shared<Value>(); }

Valptr AssignAST::Eval() { return std::make_shared<Value>(); }

Valptr ArithAST::Eval() { return std::make_shared<Value>(); }

Valptr FuncDefAST::Eval() { return std::make_shared<Value>(); }

Valptr FuncAST::Eval() { return std::make_shared<Value>(); }