#ifndef LEXER_H
#define LEXER_H

#include <istream>
#include "Token.h"

class Lexer {
    private:
        Token cur_token;
        char cur_char;
        std::istream _in;
        void nextChar() { _in >> cur_char; }

    public:
        Token HandleNum();
        Token HandleComment();
        Token HandleId();
        Token HandleStr();
};


#endif