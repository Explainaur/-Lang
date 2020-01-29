#ifndef FRONT_LEXER_DEFINE_H
#define FRONT_LEXER_DEFINE_H

#include <map>
#include <memory>
#include <vector>
#include <string>

#include "env.h"

char op[] = {
    '+', '-', '*', '/',
    '{', '}', '[', ']', 
    '=', '>', '<', '!'
};

using string = std::string;
using ASTptr = std::unique_ptr<BaseAST>;
using Valptr = std::shared_ptr<Value>;
using Args = std::vector<string>;

enum FactorType {String, Number, Identifier};

string Keyword[] = {"if", "else", "while", "def", "print"};

#endif