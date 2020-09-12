//
// Created by dyf on 2020/9/11.
//

#ifndef _LANG_TOKEN_H
#define _LANG_TOKEN_H

#include <string>

namespace Front {

    enum class TokenType {
        Keyword, Identifier, Double, Operator, EOL, Eof
    };

    class Token {
    private:
        TokenType tokenType;
        std::string keywordValue_;
        std::string identValue_;
        double numValue_;
        std::string operValue_;
        int lineNumber;
    public:
        Token() = default;

        Token(TokenType type, int line) : tokenType(type), lineNumber(line) {}

        TokenType getTokenType() {
            return tokenType;
        }

        std::string getKeywordValue() {
            return keywordValue_;
        }

        std::string getIdentValue() {
            return identValue_;
        }

        std::string getOperValue() {
            return operValue_;
        }

        double getNumValue() {
            return numValue_;
        }

        int getLineNumber() {
            return lineNumber;
        }

        void setTokenType(TokenType type) { this->tokenType = type; }

        void setKeywordValue(std::string keyword) {
            this->keywordValue_ = keyword;
        }

        void setIdentValue(std::string ident) {
            this->identValue_ = ident;
        }

        void setNumValue(double num) {
            this->numValue_ = num;
        }

        void setOperValue(std::string oper) {
            this->operValue_ = oper;
        }

        void setLineNumber(int line) {
            this->lineNumber = line;
        }

        void setToken(TokenType type, const std::string& value, int line);
    };

}

#endif //_LANG_TOKEN_H
