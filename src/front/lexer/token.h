#ifndef FRONT_LEXER_TOKEN_H
#define FRONT_LEXER_TOKEN_H

#include <string>

enum Type {String, Number, Identifier, OP, Keyword};
using string = std::string;

class Token {
    private:
        int lineNumber;
        string value;
        Type type_;

    public:
        Token(){};
        void set(int line, string val, Type type) {
            lineNumber = line;
            value = val;
            type_ = type;
        }
        Type type() { return type_; }
        string val() { return value; }
        int line() { return lineNumber; }
};

#endif