//
// Created by dyf on 2020/9/11.
//

#include <limits>
#include "lexer.h"
#include <iostream>

namespace Front {
    void Lexer::nextLine() {
        inStream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    Token Lexer::nextToken() {
        std::string value;
        Token token;
        if (ch == 0) nextChar();

        eatSpace();

        int curLine = lineNumber;
        if (!isSpace()) {
            if (ch == '#') {
                nextLine();
                lineNumber++;
                nextChar();
            }

            eatSpace();

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

                    token.setToken(TokenType::Operator, value, curLine);
                    nextChar(); // eat space
                } else if (isdigit(ch)){
                    while (!isSpace()) {
                        value += ch;
                        nextChar();
                    }
                    token.setToken(TokenType::Double, value, curLine);
                    nextChar();
                }
            } else if (isalpha(ch)) {
                while (!isSpace()) {
                    value += ch;
                    nextChar();
                }
                if (isKeyword(value)) {
                    token.setToken(TokenType::Keyword, value, curLine);
                } else {
                    token.setToken(TokenType::Identifier, value, curLine);
                }
            }

        }
        std::cout << "Token: " << value << " LineNumber: " << curLine << std::endl;
        return Token();
    }

    bool Lexer::isOper(const std::string& value) {
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
