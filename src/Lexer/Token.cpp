#include "Token.h"

Token::Token(int line, TokenType type, std::string value) {
    this -> lineNumber = line;
    this -> type = type;
    this -> value = value;
}

TokenType Token::getType(void) {
    return this -> type;
}

std::string Token::getVal(void) {
    return this -> value;
}
