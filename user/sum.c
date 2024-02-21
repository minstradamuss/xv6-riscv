#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

const int BUFSIZE = 11;

int isDigit(char ch) {
    return ('0' <= ch && ch <= '9');
}

int isEndOfLine(char ch) {
    return (ch == '\n' || ch == '\0' || ch == '\r');
}

int read_number(char* buf, int bufSize, int last) {
    int i = 0;
    while (i < bufSize) {
        int readStr = read(0, buf + i, 1);
        if (readStr == 0 || buf[i] == ' ') {
            break;
        }
        if (isEndOfLine(buf[i])) {
            if (last == 1) {
                break;
            }
            if (i == 0) {
                return -2; 
            }
            return -1; 
        }
        if (!isDigit(buf[i])) {
            return -3; 
        }
        ++i;
    }
    if (i == 0) {
        return last == 0 ? -2 : -1; 
    }
    if (bufSize == i) {
        return -2;
    }
    buf[i] = '\0';
    return 0;
}

void error(int error) {
    if (error == -1) {
        write(2, "error: reading error\n", 7 + 14);
    }
    else if (error == -2) {
        write(2, "error: input is empty\n", 7 + 14);
    }
    else if (error == -3) {
        write(2, "error: invalid syntax\n", 7 + 14);
    }
}

int main(int argc, char *argv[]) {
    char firstNum[BUFSIZE];
    char secondNum[BUFSIZE];

    int readStr = read_number(firstNum, BUFSIZE, 0);
    if (readStr < 0) {
        error(readStr);
        exit(-readStr);
    }

    readStr = read_number(secondNum, BUFSIZE, 1);
    if (readStr < 0) {
        error(readStr);
        exit(-readStr);
    }

    //printf("|%s %s|\n", firstNum, secondNum);

    printf("%d\n", atoi(firstNum) + atoi(secondNum));

    exit(0);
}
