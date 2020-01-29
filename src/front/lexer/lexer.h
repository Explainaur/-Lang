#ifndef FRONT_LEXER_H
#define FRONT_LEXER_H

#include "token.h"
#include <istream>

class Lexer {
    private:
        int cur_line = 1;
        std::istream &in;
        char cur_pos;
        Token cur_token;
        void nextChar() {
            in.get(cur_pos);
            if (cur_pos == '\n' || cur_pos == '\r')
                cur_line++;
        };
        bool isEOL() { return cur_pos == '\r' || cur_pos == '\n' || in.eof();}

    public:
        Lexer(std::istream &in_) : in(in_), cur_pos(0) { nextChar(); } 
        void set(int, string, Type);
        Token nextToken();
        Token handleComment();
        Token handleStr();
        Token handleNum();
        Token handleId();
        Token handleOP();
        Token handleEOF();
        Token handleEOL();
};

#endif