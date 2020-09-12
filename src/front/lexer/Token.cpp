//
// Created by dyf on 2020/9/11.
//

#include "Token.h"

namespace Front {

    void Token::setToken(TokenType type, const std::string &value, int line) {
        this->setTokenType(type);
        this->setLineNumber(line);
        if (type == TokenType::Double) {
            double numValue = std::stod(value);
            this->setNumValue(numValue);
        } else if (type == TokenType::Operator) {
            this->setOperValue(value);
        } else if (type == TokenType::Identifier) {
            this->setIdentValue(value);
        } else if (type == TokenType::Keyword) {
            this->setKeywordValue(value);
        }
    }
}
