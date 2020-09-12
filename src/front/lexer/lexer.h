//
// Created by dyf on 2020/9/11.
//

#ifndef _LANG_LEXER_H
#define _LANG_LEXER_H

#include "Token.h"
#include <string>
#include <fstream>

namespace Front {
    class Lexer {
    private:
        const static std::string operators[];
        const static std::string keywords[];
        char ch;
        Token currentToken;
        std::ifstream inStream;
        int lineNumber = 1;

        bool isSpace() { return std::isspace(ch) || inStream.eof(); }

    public:
        bool openFile(const std::string &filename) {
            inStream.open(filename);
            return inStream.is_open();
        }

        void nextChar() {
            if (!inStream.eof()) {
                ch = inStream.get();
                if (ch == '\n' || ch == 'r') {
                    lineNumber++;
                }
            }
        }

        void nextLine();

        Token nextToken();

        void eatSpace() {
            while (isSpace()) {
                nextChar(); // eat space
                if (inStream.eof()) break;
            }
        }

        static bool isOper(const std::string &value);

        static bool isKeyword(const std::string &value);
    };

    const std::string Lexer::operators[] = {
            "+", "-", "*", "/", "=", ">", "<",
            ">=", "<=", "==", "!=", "!", "|", "&",
            "(", ")"
    };

    const std::string Lexer::keywords[] = {
            "def", "extern", "if", "else", "then"
    };


}

#endif //_LANG_LEXER_H
