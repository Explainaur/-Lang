//
// Created by dyf on 2020/9/11.
//

#ifndef _LANG_LEXER_H
#define _LANG_LEXER_H

#include "Token.h"
#include <string>
#include <fstream>

namespace front {
    class Lexer {
    private:
        const static std::string operators[];
        const static std::string keywords[];

        char ch;
        int pos = 0;
        std::ifstream inStream;
        int lineNumber = 1;
        std::string filename;

        bool isSpace() { return std::isspace(ch) || inStream.eof(); }

    public:
        bool openFile(const std::string &file) {
            inStream.open(file);
            filename = file;
            return inStream.is_open();
        }

        void nextChar() {
            if (!inStream.eof()) {
                pos++;
                ch = inStream.get();
                if (ch == '\n' || ch == 'r') {
                    pos = 0;
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

        std::string GetFilename() { return filename; }
    };


}

#endif //_LANG_LEXER_H
