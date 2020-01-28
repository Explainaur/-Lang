#include "Lexer.h"

namespace {
    bool IsIdChar(char i) {
        return i != '=' && i != ''
    }
}

Token Lexer::HandleComment() {
    if (cur_char == '@') {
        while (!fetchEOF()) 
            nextChar();
    }
    return getToken();
}

Token Lexer::HandleNum() {
    std::string value;
    while (std::isdigit(cur_char) && !fetchEOF()) {
        value += cur_char;
        nextChar();
    }
    cur_token = Token(cur_line, TokenType::Number, value);
    return cur_token;
}

Token Lexer::HandleEOF() {
    cur_token = Token(cur_line, TokenType::__EOF__, "");
    return cur_token;
}

Token Lexer::getToken() {
    if (_in.eof()) return HandleEOF(); 
    while (!fetchEOF() && std::isspace(cur_char)) nextChar();

    if (cur_char == '@') return HandleComment();

}