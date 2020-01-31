#include "src/front/parser/parser.h"
#include "src/util/error.h"
#include <iostream>

namespace {
    const char String2char(string str) {
        return str.c_str()[0];
    }

    int String2int(string str) {
        return atoi(str.c_str());
    }

    ASTptr BuildNum(string val) {
        int value = String2int(val);
        return std::make_unique<NumAST>(value, FactorType::FNumber);
    }

    ASTptr BuildStr(string str) {
        return std::make_unique<StringAST>(str, FactorType::FString);
    }

    ASTptr BuildId(string id) {
        return std::make_unique<IdAST>(id, FactorType::FIdentifier);
    }

    ASTptr BuildAssign(ASTptr id, ASTptr val) {
        return std::make_unique<AssignAST>(move(id), move(val));
    }

    ASTptr BuildArith(string op, ASTptr left, ASTptr right) {
        return std::make_unique<ArithAST>(op, move(left), move(right),
            FactorType::FArithmetic);
    }

}

bool Parser::match(char ch) {
    return String2char(token.val()) == ch;
}

bool Parser::match(string str) {
    return token.val() == str;
}

bool Parser::match(const char * str) {
    return token.val() == string(str);
}

bool Parser::isFactor(ASTptr ptr) {
    return (ptr->type() == FactorType::FNumber) ||
    (ptr->type() == FactorType::FIdentifier);
}

bool Parser::isTermNum(ASTptr ptr) {
    return (ptr->type() == FactorType::FNumber);
}

bool Parser::isTermSameType(ASTptr left, ASTptr right) {
    switch (left->type()) {
        case FactorType::FIdentifier:
            return true;    // do check when run
        case FactorType::FunCall:
            return true;
        case FactorType::FArithmetic:
            return true;
        case FactorType::FNumber: {
            if (right->type() == FactorType::FString){
                return false;
            } else {
                return true;
            }
        }
        case FactorType::FString: {
            if (right->type() == FactorType::FNumber){
                return false;
            } else {
                return true;
            }
        }
    }   // end switch
    return false;
}

ASTptr Parser::parseAssign(Token id) {
    if (!id.type() == Type::Identifier)
        err_quit("Expect for Identifier at line %d", id.line());

    ASTptr idAST = BuildId(id.val());

    auto exprAST = parseExpr();
    if (!exprAST) return nullptr;
    return BuildAssign(move(idAST), move(exprAST));
}

ASTptr Parser::parseExpr() {

    std::cout << "EType is " << token.type2Str() << " Value is " << token.val() << std::endl;
    
    if (token.type() == Type::End)
        return nullptr;
    ASTptr temp = parseTerm();
    
    temp->print();

    ASTptr right;
    while (match('+')  || match('-')  || 
           match("==") || match("<=") ||
           match(">=") || match(">")  || 
           match("<")  || match("=")) {
        switch (String2char(token.val())) {
            case '+': {
                // eat '+'
                nextToken();
                right = parseTerm();
                if (isTermSameType(temp->clone(), right->clone())) {
                    temp = BuildArith(string("+"), move(temp), move(right));
                } else {
                    err_quit("Different type for '+' is not allow at line %d", token.line());
                }
                break;
            }
            case '-': {
                // eat '-'
                nextToken();
                right = parseTerm();
                if (isTermNum(temp->clone()) && isTermNum(right->clone())) {
                    temp = BuildArith(string("-"), move(temp), move(right));
                } else {
                    err_quit("String type for '-' is not allow at line %d", token.line());
                }
                break;
            }
            case '=': {
                // check if ==
                if ((token.val() == string("=="))) {
                    // eat '=='
                    nextToken();
                    right = parseTerm();
                    if (isTermSameType(temp->clone(), right->clone())) {
                        temp = BuildArith(string("=="), move(temp), move(right));
                    } else {
                        err_quit("Different type for == is not allow at line %d", token.line());
                    }
                } else if ((token.val() == string("="))) {
                    // eat '='
                    log_msg("assign !!! prev is %s", prev.val().c_str());
                    nextToken();
                    if (prev.type() == Type::Identifier)
                        temp = parseAssign(prev);
                    else 
                        err_quit("Assign Error at line %d", token.line());
                }
                break;
            }
            case '>': {
                // save '>' or '>='
                string op = token.val();
                // eat '>' or '>='
                nextToken();
                right = parseTerm();
                if(isTermNum(temp->clone()) && isTermNum(right->clone())) {
                    BuildArith(op, move(temp), move(right));
                } else {
                    err_quit("String type for %s is not allow at line %d",
                        op.c_str(), token.line());
                }
                break;
            }
            case '<': {
                // save '<' or '<='
                string op = token.val();
                // eat '>' or '>='
                nextToken();
                right = parseTerm();
                if(isTermNum(temp->clone()) && isTermNum(right->clone())) {
                    BuildArith(op, move(temp), move(right));
                } else {
                    err_quit("String type for %s is not allow at line %d",
                        op.c_str(), token.line());
                }
                break;
            }
        }   // switch end
    }   // while end


    log_msg("return success!\n");
    return temp;
}

ASTptr Parser::parseTerm() {
    
    std::cout << "TType is " << token.type2Str() << " Value is " << token.val() << std::endl;
    
    ASTptr temp = parseFactor();
    ASTptr right;
    while (match('*') || match('/')) {
        switch (String2char(token.val())) {
            case '*': {
                // eat '*'
                nextToken();
                right = parseFactor();
                if (isFactor(right->clone()) && isFactor(temp->clone())) {
                    temp = BuildArith(string("*"), move(temp), move(right));
                } else { 
                    err_quit("Error type to do '*' at line %d", token.line());
                }
                break;
            }
            case '/':{
                nextToken();
                right = parseFactor();
                if (isFactor(right->clone()) && isFactor(temp->clone())) {
                    temp = BuildArith(string("/"), move(temp), move(right));
                } else { 
                    err_quit("Error type to do '/' at line %d", token.line());
                }
                break;
            }
        }
    }
    return temp;
}

ASTptr Parser::parseFactor(){
    
    std::cout << "FType is " << token.type2Str() << " Value is " << token.val() << std::endl;
    
    ASTptr temp;
        if (token.type() == Type::Number) {
            temp = BuildNum(token.val());
            nextToken();
        }
        else if (token.type() == Type::String) {
            temp = BuildStr(token.val());
            nextToken();
        }
        else if (token.type() == Type::Identifier) {
            // check if function call
            prev = token;
            nextToken();
            if (match('(')) 
                temp = parseFunc();
            else
                temp = BuildId(prev.val()); 
        }
        else if (match('(')) {
            // eat '('
            nextToken();
            temp = parseExpr();
            if (!match(')'))
                err_quit("Expect for ')' at line %d", token.line());
            nextToken();
        }      
        else 
            err_quit("Can't fetch factor at line %d and value is %s" ,
                token.line(), token.val().c_str());
    return temp;
}

ASTptr Parser::parseFunc() {
    return ASTptr();
}