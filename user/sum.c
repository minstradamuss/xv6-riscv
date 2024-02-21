#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BUF_SIZE 23

int isWhitespace(char c) {
    return c == ' ' || c == '\n' || c == '\0' || c == '\r';
}

int isDigit(char c) {
    return '0' <= c && c <= '9';
}

int main(int argc, char *argv[]) {
    char newBuf[BUF_SIZE];
    gets(newBuf, BUF_SIZE);

    char* end = newBuf + BUF_SIZE - 1; char* secPtr = newBuf;

    while (secPtr != end) {
        if (isWhitespace(*secPtr)) {
            write(2, "error: there is only one number given, must be two\n", 22 + 7 + 9 + 12);
            exit(1);
        }
        if (isDigit(*secPtr)) { secPtr++; } 
        if (*secPtr == ' ') { break; }
        else {
            write(2, "error: invalid syntax\n", 14 + 7);
            exit(3);
        }
    }

    char* check = secPtr++;

    while (check < end) {
        if (isWhitespace(*check)) { break; }
        if (isDigit(*check)) { check++; } 
        else {
            write(2, "error: invalid syntax\n", 14 + 8);
            exit(3);
        }
    }

    if (check >= end) {
        write(2, "error: buffer overflow\n", 7 + 16);
        exit(2);
    }

    printf("%d\n", add(atoi(newBuf), atoi(secPtr)));
    exit(0);
}
