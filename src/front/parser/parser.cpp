#include "parser.h"
#include "../../util/error.h"

namespace {
    const char String2char(string str) {
        return str.c_str()[0];
    }

    int String2int(string str) {
        return atoi(str.c_str());
    }

    ASTptr BuildNum(string val) {
        int value = String2int(val);
        return std::make_unique<FactorAST<int>>(value, FactorType::FNumber);
    }

    ASTptr BuildStr(string str) {
        return std::make_unique<FactorAST<string>>(str, FactorType::FString);
    }

    ASTptr BuildId(string id) {
        return std::make_unique<FactorAST<string>>(id, FactorType::FIdentifier);
    }

    ASTptr BuildAssign(ASTptr id, ASTptr val) {
        return std::make_unique<AssignAST>(id, val);
    }

    ASTptr BuildArith(string op, ASTptr left, ASTptr right) {
        return std::make_unique<ArithAST>(op, left, right);
    }

}

bool Parser::match(char ch) {
    return String2char(token.val()) == ch;
}

ASTptr Parser::parseAssign(Token id) {
    ASTptr idAST = BuildId(id.val());
    // eat '='
    nextToken();

    auto exprAST = parseExpr();
    if (!exprAST) return nullptr;
    return BuildAssign(move(idAST), move(exprAST));
}

ASTptr Parser::parseExpr() {
    ASTptr tmp = parseTerm();

}

ASTptr Parser::parseTerm() {
    ASTptr temp = parseFactor();
    ASTptr right;
    while (match('*') || match('/')) {
        switch (String2char(token.val())) {
            case '*': {
                // eat '*'
                nextToken();
                right = parseFactor();
                if ((temp->type() != (FactorType::FNumber || FactorType::FIdentifier)) &&
                    (right->type() != ())) {
                    temp = BuildArith(string("*"), move(temp), move(right));
                } else { err_quit()}
                break;
            }
            case '/':{
                nextToken();
                right = parseFactor();
                if (temp->type() != right->type()) {
                    temp = BuildArith(string("/"), move(temp), move(right));
                }
                break;
            }
        }
    }
    return temp;
}

ASTptr Parser::parseFactor(){
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
            Token tmp_token = token;
            nextToken();
            if (match('(')) 
                temp = parseFunc();
            else
                temp = BuildId(tmp_token.val()); 
        }
        else if (match('(')) {
            // eat '('
            nextToken();
            temp = parseExpr();
            if (!match(')'))
                err_quit("Except for ) at line %d", token.line());
            nextToken();
        }
        else err_quit("Fetch error at line %d", token.line());
    return temp;
}