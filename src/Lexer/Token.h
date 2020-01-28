#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {String, Number, Identifier, __EOF__};

class Token {
    private:
        int lineNumber;
        TokenType type;
        std::string value;
    public:
        Token();
        Token(int, TokenType, std::string);
        TokenType getType(void);
        std::string getVal(void);
};

#endif