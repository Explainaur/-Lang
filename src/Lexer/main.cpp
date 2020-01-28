#include "Lexer.h"
#include <iostream>
#include <fstream>

int main() {
    std::fstream fd;
    fd.open("./a.dyf", std::ios::in);

    Lexer lexer = Lexer(fd);

    return 0;
}