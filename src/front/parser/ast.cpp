#include "ast.h"

template <typename T>
Valptr FactorAST<T>::Eval() {}

template <typename T, typename L, typename R>
Valptr StateAST<T, L, R>::Eval() {}

Valptr FuncDefAST::Eval() {}

Valptr FuncAST::Eval() {}