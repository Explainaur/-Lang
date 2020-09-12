//
// Created by dyf on 2020/9/11.
//

#include "Token.h"
#include<iomanip>
#include <iostream>

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

    void Token::Log() {
        if (tokenType == TokenType::Operator) {
            std::cout << std::left << std::setw(20) << "Type: Operator" << "Value: "
                      << std::left << std::setw(15)
                      << getOperValue()
                      << "LineNumber: " << getLineNumber() << std::endl;
        } else if (tokenType == TokenType::Identifier) {
            std::cout << std::left << std::setw(20) << "Type: Identifier" << "Value: "
                      << std::left << std::setw(15)
                      << getIdentValue()
                      << "LineNumber: " << getLineNumber() << std::endl;
        } else if (tokenType == TokenType::Keyword) {
            std::cout << std::left << std::setw(20) << "Type: Keyword" << "Value: "
                      << std::left << std::setw(15)
                      << getKeywordValue()
                      << "LineNumber: " << getLineNumber() << std::endl;
        } else if (tokenType == TokenType::Double) {
            std::cout << std::left << std::setw(20) << "Type: Double" << "Value: "
                      << std::left << std::setw(15)
                      << getNumValue()
                      << "LineNumber: " << getLineNumber() << std::endl;
        } else if (tokenType == TokenType::Eof) {
            std::cout << std::left << std::setw(20) << "Type: EOF" << "LineNumber: " << getLineNumber() << std::endl;
        }
    }
}
