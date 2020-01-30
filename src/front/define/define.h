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
    '<', '>', '=', '!',
    '"', '$', '~', '|',
    '(', ')', ',', '.'
};

class BaseAST;
using string = std::string;
using std::move;
using ASTptr = std::unique_ptr<BaseAST>;
using Valptr = std::shared_ptr<Value>;
using Args = std::vector<string>;
using Block = std::vector<ASTptr>;

enum FactorType {FString, FNumber, FIdentifier, Nil};
enum StatType {Assign, Arithmetic, While, If};

string keyword[] = {"if", "else", "while", "def", "print"};

#endif