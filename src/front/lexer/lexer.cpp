//
// Created by dyf on 2020/9/11.
//

#include <limits>
#include "lexer.h"
#include <iostream>

namespace front {
    const std::string Lexer::operators[] = {
            "+", "-", "*", "/", "=", ">", "<",
            ">=", "<=", "==", "!=", "!", "|", "&",
            "(", ")", ",", ";"
    };

    const std::string Lexer::keywords[] = {
            "def", "extern", "if", "else", "then"
    };

    void Lexer::nextLine() {
        pos = 0;
        inStream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    Token Lexer::nextToken() {
        std::string value;
        Token token;
        if (ch == 0) nextChar();

        eatSpace();

        int curPos = pos;
        int curLine = lineNumber;
        if (!isSpace()) {
            while (ch == '#') {
                nextLine();
                lineNumber++;
                nextChar();
                eatSpace();
            }

            if (inStream.eof()) {
                token.setToken(TokenType::Eof, "", curLine, curPos);
            }

            curPos = pos;
            curLine = lineNumber;
            if (value.length() == 0) {
                if (isOper(std::string(1, ch))) {
                    value += ch;

                    /* Check if >=, <=, ==, != */
                    if (ch == '>' || ch == '<' || ch == '=' || ch == '!') {
                        nextChar();
                        if (ch == '=') {
                            value += ch;
                        }
                    }

                    token.setToken(TokenType::Operator, value, curLine, curPos);
                    nextChar(); // eat space
                } else if (isdigit(ch)) {
                    while (!isSpace()) {
                        value += ch;
                        nextChar();

                        if (isOper(std::string(1, ch))) {
                            break;
                        }
                    }
                    token.setToken(TokenType::Double, value, curLine, curPos);
                } else if (isalpha(ch)) {
                    while (!isSpace()) {
                        value += ch;
                        nextChar();

                        if (isOper(std::string(1, ch))) {
                            break;
                        }
                    }
                    if (isKeyword(value)) {
                        token.setToken(TokenType::Keyword, value, curLine, curPos);
                    } else {
                        token.setToken(TokenType::Identifier, value, curLine, curPos);
                    }
                }
            }

        } else if (ch == -1) {
            token.setToken(TokenType::Eof, "", curLine, curPos);
        }
//        token.Log();
        return token;
    }

    bool Lexer::isOper(const std::string &value) {
        for (auto i : Lexer::operators) {
            if (i == value) {
                return true;
            }
        }
        return false;
    }

    bool Lexer::isKeyword(const std::string &value) {
        for (auto i : Lexer::keywords) {
            if (i == value) {
                return true;
            }
        }
        return false;
    }
}
