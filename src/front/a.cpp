#include <iostream>
#include <string>

using namespace std;

const char String2char(string str) {
    return str.c_str()[0];
}

bool match(char ch, string str) {
    return String2char(str) == ch;
}

int main() {
    string strg = "{";
    char ch = '{';
    if (match(ch, strg))    cout<<"yes"<<endl;
}
