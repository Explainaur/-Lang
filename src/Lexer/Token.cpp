#include "Token.h"

Token::Token(int line) {
    this -> lineNumber = line;
}

TokenType Token::getType(void) {
    return this -> type;
}

std::string Token::getVal(void) {
    return this -> value;
}

void Token::setType(TokenType type) {
    this -> type = type;
}

void Token::set(std::string val) {
    this -> value = val;
}