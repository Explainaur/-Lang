#ifndef LEXER_H
#define LEXER_H

#include <istream>
#include "Token.h"

class Lexer {
    private:
        int cur_line;
        Token cur_token;
        char cur_char;
        std::istream &_in;
        void nextChar() { _in >> cur_char; }
        bool fetchEOF() {
            if (_in.eof() || cur_char == '\n' || cur_char == '\r') {
                cur_line++;
                return true;
            }
            return false;
        }
    public:
        Lexer(std::istream &in) : cur_line(0), _in(in){}
        Token getToken();
        Token HandleNum();
        Token HandleComment();
        Token HandleId();
        Token HandleStr();
        Token HandleEOF();
};


#endif