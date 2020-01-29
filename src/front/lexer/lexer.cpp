#include "lexer.h"
#include "define.h"
#include "../../util/error.h"

namespace {
    bool isOPChar(char ch) {
        for(char i : op) {
            if (i == ch) return true;
        }
        return false;
    }
    bool isIdChar(char ch) {
        return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch >= 'Z') || (ch >= '0' && ch <= '9') && !isOPChar(ch));
    }
}

void Lexer::set(int line, string val, Type type) {
    cur_token.set(line, val, type);
}

Token Lexer::nextToken() {
    if (in.eof()) return handleEOF();
    // eat space
    while (!isEOL() && (std::isspace(cur_pos) || cur_pos == 2)) nextChar();

    if (cur_pos == '?') return handleComment();
    if (std::isdigit(cur_pos)) return handleNum();
    if (isIdChar(cur_pos)) return handleId();
    if (cur_pos == '"') return handleStr();
    if (isOPChar(cur_pos)) return handleOP();
    if (isEOL()) return handleEOL();

    err_quit("invalid token at line %d, the char is %c", cur_line, cur_pos);
    return cur_token;
}

Token Lexer::handleEOF() {
    set(cur_line, "\n", Type::String);
    return cur_token;
}

Token Lexer::handleComment() {
    nextChar();
    while (!isEOL()) nextChar();
    return nextToken();
}

Token Lexer::handleNum() {
    string val;
    while (!isEOL() && std::isdigit(cur_pos)) {
        val += cur_pos;
        nextChar();
    }
    set(cur_line, val, Type::Number);
    return cur_token;
}

Token Lexer::handleId() {
    string id;
    while (!isEOL() && isIdChar(cur_pos)) {
        id += cur_pos;
        nextChar();
    }
    set(cur_line, id, Type::Identifier);
    return cur_token;
}

Token Lexer::handleStr() {
    string val;
    // eat '"'
    nextChar();
    while (cur_pos != '"') {
        val += cur_pos;
        nextChar();
    }
    set(cur_line, val, Type::String);
    nextChar();
    return cur_token;
}

Token Lexer::handleOP() {
    string val;
    val += cur_pos;
    // check if '>=' '==' '<='
    switch (cur_pos) {
        case '=':
            nextChar();
            (cur_pos == '=') ? val += cur_pos : val;
            break;
        case '>':
            nextChar();
            (cur_pos == '=') ? val += cur_pos : val;
            break;
        case '<':
            nextChar();
            (cur_pos == '=') ? val += cur_pos : val;
            break;
    }
    set(cur_line, val, Type::OP);
    return cur_token;
}

Token Lexer::handleEOL() {
    while (isEOL() && !in.eof()) nextChar();
    return nextToken();
}