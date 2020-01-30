#include "src/front/lexer/lexer.h"
#include "src/front/define/define.h"
#include "src/util/error.h"

namespace {
    bool IsOPChar(char ch) {
        for(char i : oper) {
            if (i == ch) return true;
        }
        return false;
    }

    bool IsIdChar(char ch) {
        return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch >= 'Z') || (ch >= '0' && ch <= '9') && !IsOPChar(ch));
    }

    bool IsComment(char ch) {
        return (ch == '@' || ch == '?');
    }

    bool IsKeyword(string str) {
        for(string i : keyword) {
            if (i == str) return true;
        }
        return false;
    }
}

void Lexer::set(int line, string val, Type type) {
    cur_token.set(line, val, type);
}

Token Lexer::nextToken() {
    if (in.eof()) return handleEOF();
    // eat space
    while (!isEOL() && (std::isspace(cur_pos) || cur_pos == 2)) nextChar();

    if (IsComment(cur_pos)) return handleComment();
    if (std::isdigit(cur_pos)) return handleNum();
    if (IsIdChar(cur_pos)) return handleId();
    if (cur_pos == '"') return handleStr();
    if (IsOPChar(cur_pos)) return handleOP();
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

/*
 * FIXME: Support negetive number.
 */
Token Lexer::handleNum() {
    int line;
    string val;
    while (!isEOL() && std::isdigit(cur_pos)) {
        val += cur_pos;
        nextChar();
    }
    (isEOL() && !in.eof()) ? line = cur_line - 1 : line = cur_line;
    set(line, val, Type::Number);
    return cur_token;
}

Token Lexer::handleId() {
    int line;
    string id;
    Type type;
    while (IsIdChar(cur_pos) && !in.eof()) {
        id += cur_pos;
        nextChar();
    }
    isEOL() ? line = cur_line - 1 : line = cur_line;
    IsKeyword(id) ? type = Type::Keyword : Type::Identifier;
    set(line, id, type);
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
    int line;
    string val;
    val += cur_pos;
    // check if '>=' '==' '<='
    switch (cur_pos) {
        case '=': {
            nextChar();
            (cur_pos == '=') ? val += cur_pos : val;
            break;
        }
        case '>': {
            nextChar();
            (cur_pos == '=') ? val += cur_pos : val;
            break;
        }
        case '<': {
            nextChar();
            (cur_pos == '=') ? val += cur_pos : val;
            break;
        }
    }
    set(cur_line, val, Type::OP);
    nextChar();
    return cur_token;
}

Token Lexer::handleEOL() {
    while (isEOL() && !in.eof()) nextChar();
    return nextToken();
}